#pragma once

#include <mpi.h>

#include "LocalVector.h"
#include "Matrix.h"

template <typename T>
class DistMatrix : public virtual Matrix<T>
{
public:

    int localNumCols;

    DistMatrix()
    {
        localNumCols = -1;
    }

    virtual ~DistMatrix()
    {
        localNumCols = -1;
    }
    
protected:

    // Buffer for gathering a distributed vector to
    LocalVector<T> x_tmp;
};