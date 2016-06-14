#pragma once

#include "Container.h"


template <typename T>
class DistVector;

template <typename T>
class Matrix : public Container<T>
{
public:

    int numRows;
    int numCols;
    
    Matrix()
    {
        numRows = -1;
        numCols = -1;
    }

    virtual ~Matrix()
    {
        numRows = -1;
        numCols = -1;
    }
};