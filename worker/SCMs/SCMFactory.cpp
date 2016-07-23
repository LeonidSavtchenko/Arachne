#include "SCMFactory.h"

#include "MatFileIO/MatFileIOUtils.h"
#include "Containers/LocalVector.h"
#include "Containers/DistVector.h"
#include "DistEnv.h"
#include "ScmType.h"

#include <iostream>
#include <random>   // mt19937, mt19937_64
#include <string>

// All the distributed matrix classes that derive from SCMBase class
#include "SimpleSCM.h"
#include "InPlaceSCM.h"
#include "OneBitSCM.h"
#include "ZeroSCM.h"
#include "UniformSCM.h"
#include "SparseSCM.h"


using namespace std;


// Create distributed matrix based on MAT-file "input.mat."
// If scmType is equal to ScmType::KrnDense, ScmType::KrnSparse, ScmType::KrnOneBit or ScmType::KrnInPlace,
// then each matrix is initialized with other set of seeds.
// The function is called on all processes, but on master thread only.
template <typename T, typename RNGT>
SCMBase<T, RNGT>* CreateSCM(
    const char *suf,
    int numRows,
    int numCols,
    SclModel sclModel,
    ScmType scmType,
    DistVector<int> *pScmSeedsDist,
    LocalDenseMatrix<T, RNGT> &local_g,
    bool useLocal)
{
    using namespace DistEnv;

    SCMBase<T, RNGT> *pSCM = nullptr;

    string name = "g_";
    name += suf;

    LocalVector<T> releaseProb = ReadReleaseProb<T>(suf, numRows);
        
    switch (scmType)
    {
    case ScmType::AllZeros:
        {
            // Create matrix with all elements equal zero
            pSCM = new ZeroSCM<T, RNGT>(numRows, numCols);
            break;
        }

    case ScmType::AllEqual:
        {
            // Create matrix with all equal elements
            string g_hat_name = "g_hat_";
            g_hat_name += suf;
            T g_hat = ReadCheckBroadcastScalar<T>(g_hat_name.c_str());
            T value = g_hat;

            pSCM = new UniformSCM<T, RNGT>(numRows, numCols, releaseProb, value);
            break;
        }

    case ScmType::HstDense:
        {
            // Read matrix with dense structure from input
            // MAT-file on master rank, scatter it to all ranks

            if (myRank == MASTER_RANK)
            {
                string msg = string("Reading and scattering ") + name + " ...\n";
                cout << msg.c_str() << flush;
            }
            
            DistDenseMatrix<T, RNGT> distMatrix;
            if (useLocal)
            {
                ReadCheckInMatrix<T, RNGT>(local_g, name.c_str(), numRows, numCols, MatFile::Input);
                distMatrix = local_g.Scatter();
            }
            else
            {
                distMatrix = ReadCheckScatterMatrix<T, RNGT>(name.c_str(), numRows, numCols);
            }
            
            string max_name = "g_max_";
            max_name += suf;
            T max_value = ReadCheckBroadcastScalar<T>(max_name.c_str());

            auto pSimpleSCM = new SimpleSCM<T, RNGT>(distMatrix);
            pSimpleSCM->max_value = max_value;
            pSimpleSCM->rvg = vector<BoostedBernoulliDistribution<T, RNGT>>(numRows);
    
            for (int j = 0; j < numRows; j++)
            {
                pSimpleSCM->rvg[j] = BoostedBernoulliDistribution<T, RNGT>(releaseProb[j]);
            }

            pSCM = pSimpleSCM;
            break;
        }

    case ScmType::HstSparse:
        {
            // Read matrix from input MAT-file on master rank,
            // scatter it to all ranks

            if (myRank == MASTER_RANK)
            {
                string msg = string("Reading and scattering ") + name + " ...\n";
                cout << msg.c_str() << flush;
            }

            DistSparseMatrix<T, RNGT> distSparseMatrix = ReadCheckScatterSparseMatrix<T, RNGT>(name.c_str(), numRows, numCols);
            pSCM = new SparseSCM<T, RNGT>(distSparseMatrix);
            pSCM->rvg = vector<BoostedBernoulliDistribution<T, RNGT>>(numRows);
    
            for (int j = 0; j < numRows; j++)
            {
                pSCM->rvg[j] = BoostedBernoulliDistribution<T, RNGT>(releaseProb[j]);
            }
            break;
        }

    case ScmType::KrnDense:
        {
            // Generate matrix just once before 1st iteration already in distributed form.

            string g_hat_name = "g_hat_";
            g_hat_name += suf;
            T g_hat = ReadCheckBroadcastScalar<T>(g_hat_name.c_str());

            string w_max_name = "w_";
            w_max_name += suf;
            w_max_name += "_max";
            T w_max = ReadCheckBroadcastScalar<T>(w_max_name.c_str());
            
            string sigma_name = "sigma_";
            sigma_name += suf;
            T sigma = ReadCheckBroadcastScalar<T>(sigma_name.c_str());
            
            bool isOneType = suf[0] == suf[1];

            if (myRank == MASTER_RANK)
            {
                string msg = "Generating " + name + " ...\n";
                cout << msg.c_str() << flush;
            }

            pSCM = new SimpleSCM<T, RNGT>(numRows, numCols, w_max, g_hat, sigma, releaseProb, sclModel, isOneType, *pScmSeedsDist);
            break;
        }

    case ScmType::KrnSparse:
        {
            // Generate sparse matrix just once before 1st iteration already in distributed form.

            string g_hat_name = "g_hat_";
            g_hat_name += suf;
            T g_hat = ReadCheckBroadcastScalar<T>(g_hat_name.c_str());

            string w_max_name = "w_";
            w_max_name += suf;
            w_max_name += "_max";
            T w_max = ReadCheckBroadcastScalar<T>(w_max_name.c_str());
            
            string sigma_name = "sigma_";
            sigma_name += suf;
            T sigma = ReadCheckBroadcastScalar<T>(sigma_name.c_str());
            
            bool isOneType = suf[0] == suf[1];

            if (myRank == MASTER_RANK)
            {
                string msg = "Generating " + name + " ...\n";
                cout << msg.c_str() << flush;
            }

            pSCM = new SparseSCM<T, RNGT>(numRows, numCols, w_max, g_hat, sigma, releaseProb, isOneType, *pScmSeedsDist);
            break;
        }
    case ScmType::KrnOneBit:
        {
            // Generate matrix just once before 1st iteration already in distributed form.
            // Use bit representation of the matrix.

            string g_hat_name = "g_hat_";
            g_hat_name += suf;
            T g_hat = ReadCheckBroadcastScalar<T>(g_hat_name.c_str());

            string w_max_name = "w_";
            w_max_name += suf;
            w_max_name += "_max";
            T w_max = ReadCheckBroadcastScalar<T>(w_max_name.c_str());
            
            string sigma_name = "sigma_";
            sigma_name += suf;
            T sigma = ReadCheckBroadcastScalar<T>(sigma_name.c_str());
            
            bool isOneType = suf[0] == suf[1];

            if (myRank == MASTER_RANK)
            {
                string msg = "Generating " + name + " ...\n";
                cout << msg.c_str() << flush;
            }

            pSCM = new OneBitSCM<T, RNGT>(numRows, numCols, w_max, g_hat, sigma, releaseProb, isOneType, *pScmSeedsDist);

            break;
        }
    case ScmType::KrnInPlace:
        {
            // The synaptic conductance matrix will be generated in place on each iteration

            string g_hat_name = "g_hat_";
            g_hat_name += suf;
            T g_hat = ReadCheckBroadcastScalar<T>(g_hat_name.c_str());

            string w_max_name = "w_";
            w_max_name += suf;
            w_max_name += "_max";
            T w_max = ReadCheckBroadcastScalar<T>(w_max_name.c_str());
            
            string sigma_name = "sigma_";
            sigma_name += suf;
            T sigma = ReadCheckBroadcastScalar<T>(sigma_name.c_str());
            
            bool isOneType = suf[0] == suf[1];

            pSCM = new InPlaceSCM<T, RNGT>(numRows, numCols, w_max, g_hat, sigma, releaseProb, sclModel, isOneType, *pScmSeedsDist);
            break;
        }

    default:
        {
            if (myRank == MASTER_RANK)
            {
                cout << "\n>>>>> Not supported scmType: " << scmType << ".\n" << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
        } 
    }

    if (pSCM == nullptr && myRank == MASTER_RANK)
    {
        cout << "\n>>>>> Unexpected error occurred during distributed matrix creation.\n" << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    return pSCM;
}

template <typename T, typename RNGT>
SCMBase<T, RNGT>* ReadSimpleSCM(
    const char *suf,
    int numRows,
    int numCols,
    SclModel sclModel,
    LocalDenseMatrix<T, RNGT> &local_g,
    bool gatherSCM)
{
    using namespace DistEnv;

    SimpleSCM<T, RNGT> *pSimpleSCM = nullptr;

    string name = "g_";
    name += suf;
    
    if (gatherSCM)
    {
        ReadCheckInMatrix<T, RNGT>(local_g, name.c_str(), numRows, numCols, MatFile::Output);
    }
    else
    {
        ReadCheckInMatrix<T, RNGT>(local_g, name.c_str(), numRows, numCols, MatFile::IntermInput);
    }
    DistDenseMatrix<T, RNGT> distMatrix = local_g.Scatter();
    pSimpleSCM = new SimpleSCM<T, RNGT>(distMatrix);

    string max_name = "g_max_";
    max_name += suf;
    pSimpleSCM->max_value = ReadCheckBroadcastScalar<T>(max_name.c_str());

    LocalVector<T> releaseProb = ReadReleaseProb<T>(suf, numRows);
    
    pSimpleSCM->rvg = vector<BoostedBernoulliDistribution<T, RNGT>>(numRows);
    
    for (int j = 0; j < numRows; j++)
    {
        pSimpleSCM->rvg[j] = BoostedBernoulliDistribution<T, RNGT>(releaseProb[j]);
    }
    
    return pSimpleSCM;
}

// Read release probabilities
template <typename T>
LocalVector<T> ReadReleaseProb(string suf, int length)
{
    LocalVector<T> releaseProb;
    string releaseProb_name = "releaseProb_";
    releaseProb_name += suf;
    if (suf[0] == suf[1])
    {
        T prob = ReadCheckBroadcastScalar<T>(releaseProb_name.c_str());
        releaseProb = LocalVector<T>(length);
        for (int i = 0; i < length; i++)
        {
            releaseProb[i] = prob;
        }
    }
    else
    {
        releaseProb = ReadCheckBroadcastVector<T>(releaseProb_name.c_str(), length);
    }
    return releaseProb;
}


template
SCMBase<float, mt19937>* CreateSCM<float, mt19937>(const char *suf, int numRows, int numCols, SclModel sclModel, ScmType scmType, DistVector<int> *pScmSeedsDist, LocalDenseMatrix<float, mt19937> &local_g, bool useLocal);

template
SCMBase<double, mt19937>* CreateSCM<double, mt19937>(const char *suf, int numRows, int numCols, SclModel sclModel, ScmType scmType, DistVector<int> *pScmSeedsDist, LocalDenseMatrix<double, mt19937> &local_g, bool useLocal);

template
SCMBase<double, mt19937_64>* CreateSCM<double, mt19937_64>(const char *suf, int numRows, int numCols, SclModel sclModel, ScmType scmType, DistVector<int> *pScmSeedsDist, LocalDenseMatrix<double, mt19937_64> &local_g, bool useLocal);


template
SCMBase<float, mt19937>* ReadSimpleSCM<float, mt19937>(const char *suf, int numRows, int numCols, SclModel sclModel, LocalDenseMatrix<float, mt19937> &local_g, bool gatherSCM);

template
SCMBase<double, mt19937>* ReadSimpleSCM<double, mt19937>(const char *suf, int numRows, int numCols, SclModel sclModel, LocalDenseMatrix<double, mt19937> &local_g, bool gatherSCM);

template
SCMBase<double, mt19937_64>* ReadSimpleSCM<double, mt19937_64>(const char *suf, int numRows, int numCols, SclModel sclModel, LocalDenseMatrix<double, mt19937_64> &local_g, bool gatherSCM);


template
LocalVector<float> ReadReleaseProb<float>(string suf, int length);

template
LocalVector<double> ReadReleaseProb<double>(string suf, int length);