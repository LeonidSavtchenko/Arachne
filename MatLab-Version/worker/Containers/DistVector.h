#pragma once

#include <random>

#include <mpi.h>
#include "DistributionWrapper.h"
#include "Vector.h"

template <typename T>
class LocalVector;

template <typename T, typename RNGT>
class LocalDenseMatrix;

template <typename T>
class DistVector : public Vector<T>
{
public:

    // Data
    T *localData;
    int localLength;

    // Default constructor
    DistVector();

    // Custom constructor
    DistVector(int length);

    // Inlined operator []
    
    inline T& operator [] (int localIdx)
    {
        return localData[localIdx];
    }

    inline const T& operator [] (int localIdx) const
    {
        return localData[localIdx];
    }

    // Methods

    // Create a deep copy of this vector.
    // (The method should be called on all ranks with single thread.)
    DistVector<T> Clone() const;

    // Assign zeros to all elements of this vector and return a reference to it.
    // (The method should be called on all ranks with multiple threads.)
    DistVector<T>& AssignZeros();

    // Populate distributed vector with random numbers,
    // update the states of random number generators.
    template <typename RNGT>
    DistVector<T>& FillRandom(DistributionWrapper &minToMax, DistVector<RNGT> &states);    

    // Given distributed vector, gather data following the mode specified by onlyMaster flag:
    // false - gather into local vectors preallocated on each rank
    //         (local vectors with the same data live on each rank after gathering);
    // true  - gather only into local vector living on master rank.
    void Gather(LocalVector<T> &vector, bool onlyMaster = false) const;
    
    // Given distributed vector, do incremental gathering from all ranks to local vector living on master rank.
    //
    // Input arguments:
    // localCounter - number of elements to be gathered from given rank
    //                (the elements with indices 0, 1, ..., localCounter - 1 will be gathered,
    //                localCounter must not exceed this->localLength);
    // startIdx     - index of the first element in vector to gather to
    //                (the first element gathered from the first rank will be placed into position startIdx,
    //                the first element gathered from the second rank will be placed into position startIdx + localCounter_for_the_first_rank
    //                and so forth).
    //                This argument is read only on master rank and updated after gathering.
    //
    // Left-hand output argument:
    // True if success, false if no enough space in the target vector to gather to.
    bool Gather(LocalVector<T> &vector, int localCounter, int &startIdx) const;

    // Populate entire column of the local matrix living on the master rank with selected elements of this distributed vector.
    // The vector "srcIdxs" must be same on all ranks.
    // The number of rows in "dstMatrix" must be equal to the length of "srcIdxs".
    template <typename RNGT>
    void CopySliceToMatrixColumn(
        /*out*/ LocalDenseMatrix<T, RNGT> &dstMatrix,
        int dstColIdx,
        const LocalVector<int> &srcIdxs) const;

    // Destructor
    ~DistVector();
};