#pragma once

#include "GetTypeTagUtils.h"

template <typename T>
class Container
{
public:

    MPI_Datatype mpiTypeTag;

    Container()
    {
        mpiTypeTag = GetMpiDataType<T>();
    }

    virtual ~Container()
    {
        mpiTypeTag = (MPI_Datatype)0;
    }
};