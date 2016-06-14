#include "SCMBase.h"
#include "ScmType.h"
#include "SclModel.h"

#include <string>


// Create distributed matrix based on MAT-file "input.mat."
// If scmType is equal to ScmType::KrnDense, ScmType::KrnSparse, ScmType::KrnOneBit, ScmType::KrnTwoBits or ScmType::KrnInPlace,
// then each matrix is initialized with other set of seeds.
// The function is called on all processes, but on master thread only.
template <typename T, typename RNGT>
SCMBase<T, RNGT>* CreateSCM(
    const char* suf,
    int numRows,
    int numCols,
    SclModel sclModel,
    ScmType scmType,
    DistVector<int> *pScmSeedsDist,
    LocalDenseMatrix<T, RNGT> &local_g,
    bool useLocal);

// Read distributed matrix from MAT-file "input.mat."
// The function is called on all processes, but on master thread only.
template <typename T, typename RNGT>
SCMBase<T, RNGT>* ReadSimpleSCM(
    const char* suf,
    int numRows,
    int numCols,
    SclModel sclModel,
    LocalDenseMatrix<T, RNGT> &local_g,
    bool gatherSCM);

// Read release probabilities
template <typename T>
LocalVector<T> ReadReleaseProb(std::string suf, int length);