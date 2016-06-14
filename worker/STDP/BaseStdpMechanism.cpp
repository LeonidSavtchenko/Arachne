#include <omp.h>

#include "BaseStdpMechanism.h"
#include "MatFileIO/MatFileIOUtils.h"

template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT> BaseStdpMechanism<T, RNGT>::stdpCommonParams;

template <typename T, typename RNGT>
void BaseStdpMechanism<T, RNGT>::ReadInputDataAllocateTemporaryArrays()
{
    if (stdpCommonParams.data == nullptr)
    {
        int stdpCommonParamsNum = ReadCheckBroadcastScalar<int>("stdpCommonParamsNum");
        stdpCommonParams = ReadCheckBroadcastMatrix<T, RNGT>("stdpCommonParams", stdpCommonParamsNum, 4);
    }

    stdpPeriodIter = ReadCheckBroadcastScalar<int>("stdpPeriodIter");
    windowedSTDP = ReadCheckBroadcastScalar<bool>("windowedSTDP");
    if (windowedSTDP)
    {
        T epsAt0p5T = ReadCheckBroadcastScalar<T>("epsAt0p5T");
        s = -3 * log(epsAt0p5T);
    }
}
  
// Hebbian correction
template <typename T, typename RNGT>
void BaseStdpMechanism<T, RNGT>::HebbianCorrection(
    SimpleSCM<T, RNGT>* scm,
    const LocalVector<T> &times1,
    const LocalVector<int> &indexes1,
    int num1,
    const LocalVector<T> freq1, 
    const LocalVector<T> &times2,
    const LocalVector<int> &indexes2,
    int num2,
    const DistVector<T> freq2,
    LocalDenseMatrix<T, RNGT> &copy_col, T t_new)
{
    using namespace DistEnv;
    
    int numRows = scm->numRows;
    int numCols = scm->localNumCols;
    int totalNumCols = scm->numCols;
    int startIdx = GetRankChunkStartIdx(totalNumCols, myRank);
        
    T *matData = scm->localData;
    
    const T *t1 = times1.data;
    const T *t2 = times2.data;
    const int *i1 = indexes1.data;
    const int *i2 = indexes2.data;
    
    int myThread = omp_get_thread_num();
    int yStartIdx = GetThreadChunkStartIdx(numCols, myThread);
    int yEndIdx = GetThreadChunkStartIdx(numCols, myThread + 1);
    
    // Loop by SCM columns
    for (int j = yStartIdx; j < yEndIdx; j++)
    {
        T *copyData = copy_col.data + myThread * numRows;
        memcpy(copyData, matData + j * numRows, numRows * sizeof(T));
        
        T freq2_j = freq2[j - yStartIdx];

        // Loop by spikes of 2nd network
        for (int idx2 = 0; idx2 < num2; idx2++)
        {
            if (i2[idx2] == startIdx + j)
            {
                T t2_idx2 = t2[idx2];

                // Loop by SCM rows
                for (int i = 0; i < numRows; i++)
                {
                    T m_ij = copyData[i];
                    T freq1_i = freq1[i];

                    // Loop by spikes of 1st network
                    for (int idx1 = 0; idx1 < num1; idx1++)
                    {
                        if (i1[idx1] == i)
                        {
                            matData[j * numRows + i] += AtomicCorrection(m_ij, t1[idx1], t2_idx2, freq1_i, freq2_j, t_new);
                        }
                    }
                }
            }
        }
        for (int i = 0; i < numRows; i++)
        {
            T &value = matData[j * numRows + i];
            if (value < 0)
            {
                value = 0;
            }

            else if (value > scm->max_value)
            {
                value = scm->max_value;
            }
        }
    }
}


template
class BaseStdpMechanism<float, std::mt19937>;

template
class BaseStdpMechanism<double, std::mt19937>;

template
class BaseStdpMechanism<double, std::mt19937_64>;
