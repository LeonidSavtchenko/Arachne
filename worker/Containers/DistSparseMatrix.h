#pragma once

#include "DistVector.h"
#include "DistMatrix.h"
#include "LocalDenseMatrix.h"

#include <random>
#include <matrix.h>
#include <mpi.h>

// The class that represents a matrix of synaptic conductance with sparse structure.
// Contains only nonzero elements and their indexes.
template <typename T, typename RNGT>
class DistSparseMatrix : public virtual DistMatrix<T>
{
public:

    // Array with nonzero elements of the matrix
    T *sr;

    // Array with row indexes of the elements in the matrix
    int *ir;

    // Array of index of first nonzero element of column i.
    // For example jc[1] = 3, jc[2] = 5, so in the column number 1
    // there are two nonzero elements with row indexes ir[3] and ir[4] respectively.
    int *jc;

public:
    
    // Default constructor
    DistSparseMatrix();

    // Custom constructor
    DistSparseMatrix(int numElem, int numRows, int numCols);

    // Destructor
    virtual ~DistSparseMatrix();

};