#pragma once

#include "DistVector.h"
#include "LocalMatrix.h"
#include "DenseMatrix.h"


// Memory allocation mode for LocalDenseMatrix<T, RNGT> constructor
enum AllocModeMatrix
{
    MasterMxCalloc,         // Allocate only on master rank with mxCalloc
                            // (must be used for objects that are going to be saved into MAT-file),
    MasterMxCallocOtherNew  // Allocate on master rank with mxCalloc, on other ranks with new T[]
                            // (must be used for objects that are going to be saved into MAT-file)
};


template <typename T, typename RNGT>
class DistDenseMatrix;


template <typename T, typename RNGT>
class LocalDenseMatrix : public LocalMatrix<T>, public DenseMatrix<T>
{
public:

    // Data
    T *data;

    // Default constructor
    LocalDenseMatrix();

    // Custom constructor
    LocalDenseMatrix(T *data, int numRows, int numCols);
    
    // Custom constructor 2: Allocate memory on this rank with new T[]
    LocalDenseMatrix(int numRows, int numCols);

    // Custom constructor 3: Allocate memory on master rank or all ranks with new T[] or mxCalloc
    LocalDenseMatrix(int numRows, int numCols, AllocModeMatrix allocMode);
    
    // Fill local matrix with zeros
    void FillZeros();
    
    // Fill one column of the local matrix (matrices) with data from the distributed vector.
    // If onlyMaster == true, then only matrix on master rank receives the data.
    // If onlyMaster == false, then matrices on all ranks receive the data (the matrices are supposed to be clones).
    void FillColFromVector(const DistVector<T> &vector, int colIdx, bool onlyMaster) override;
    
    // Scatter from rank 0 to all ranks so that each rank contained some number of columns
    DistDenseMatrix<T, RNGT> Scatter() const;

    // Destructor
    ~LocalDenseMatrix();
};