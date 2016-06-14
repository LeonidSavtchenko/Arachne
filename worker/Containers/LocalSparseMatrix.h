#pragma once

#include <random>
#include <matrix.h>
#include <mpi.h>
#include "LocalMatrix.h"
#include "DistSparseMatrix.h"


template <typename T, typename RNGT>
class LocalSparseMatrix : public LocalMatrix<T>
{
public:

    // Data
    T *sr;
    int *ir;
    int *jc;
    int numElem;

    // Default constructor.
    LocalSparseMatrix();

    // Custom constructor.
    LocalSparseMatrix(mxArray *data);
    
    // Destructor.
    ~LocalSparseMatrix();

    // Scatter from rank 0 to all ranks so that each rank contained some number of columns
    DistSparseMatrix<T, RNGT> Scatter() const;
};