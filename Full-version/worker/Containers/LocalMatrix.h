#pragma once

#include "Matrix.h"

template <typename T>
class LocalMatrix : public virtual Matrix<T>
{
public:

    virtual ~LocalMatrix()
    {
    }
};