#pragma once

#include "LocalVector.h"
#include "DistVector.h"
#include "LocalDenseMatrix.h"
#include "DistMatrix.h"
#include "DenseMatrix.h"
#include <mpi.h>


template <typename T, typename RNGT>
class DistDenseMatrix : public virtual DistMatrix<T>, public DenseMatrix<T>
{
public:

    // Data
    T *localData;
    
    // Default constructor
    DistDenseMatrix();

    // Custom constructor.
    // Distributed matrix is created given number of rows and columns.
    // Each rank contains all rows and some number of columns.
    DistDenseMatrix(int numRows, int numCols);

    // Fill distributed matrix with zeros
    void FillZeros();
    
    // Fill one column of the matrix with data from distributed vector
    void FillColFromVector(const DistVector<T> &vector, int colIdx, bool placeholder) override;

    // Fill one row of the matrix with data from distributed vector
    void FillRowFromVector(const DistVector<T> &vector, int rowIdx);

    // Gather data into local matrix living on master rank
    void Gather(LocalDenseMatrix<T, RNGT> &matrix);
    
    // Populate entire column of the local matrix living on the master rank with selected elements of this distributed matrix.
    // The matrix "srcRowColIdxs" must be same on all ranks.
    // The number of rows in "dstMatrix" must be equal to the number of rows in "srcRowColIdxs".
    void CopySliceToMatrixColumn(
        /*out*/ LocalDenseMatrix<T, RNGT> &dstMatrix,
        int dstColIdx,
        const LocalDenseMatrix<int, RNGT> &srcRowColIdxs) const;

    // Destructor
    virtual ~DistDenseMatrix();
};