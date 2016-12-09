#pragma once

#include "DistVector.h"
#include "Matrix.h"

template <typename T>
class DenseMatrix : public virtual Matrix<T>
{
public:

    // Pure virtual method which fills one column of the matrix with data from the distributed vector
    virtual void FillColFromVector(const DistVector<T> &vector, int colIdx, bool placeholder = false) = 0;

};