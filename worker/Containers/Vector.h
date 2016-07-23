#pragma once

#include "Container.h"


template <typename T>
class Vector : public Container<T>
{
public:

    int length;

    Vector()
    {
        length = -1;
    }

    virtual ~Vector()
    {
        length = -1;
    }
};