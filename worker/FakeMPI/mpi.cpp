#include "mpi.h"

#include <iostream>
#include <assert.h>


int MPIAPI MPI_Comm_size(MPI_Comm, int *numRanks)
{
    *numRanks = 1;

    return 1;
}

int MPIAPI MPI_Comm_rank(MPI_Comm, int *myRank)
{
    *myRank = 0;

    return 0;
}

int MPIAPI MPI_Type_contiguous(int, MPI_Datatype, MPI_Datatype *)
{
    return 0;
}

int MPIAPI MPI_Type_commit(MPI_Datatype *)
{
    return 0;
}

int MPIAPI MPI_Send(void*, int, MPI_Datatype, int, int, MPI_Comm)
{
    return 0;
}

int MPIAPI MPI_Recv(void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status *)
{
    return 0;
}

int MPIAPI MPI_Barrier(MPI_Comm)
{
    return 0;
}

int MPIAPI MPI_Reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int, MPI_Comm)
{
    if (op == MPI_SUM && datatype == MPI_INT && count == 1)
    {
        *(int*)recvbuf = *(int*)sendbuf;
    }
    else
    {
        assert(false);
    }
    
    return 0;
}

int MPIAPI MPI_Bcast(void*, int, MPI_Datatype, int, MPI_Comm)
{
    return 0;
}

int MPIAPI MPI_Init(int *, char ***)
{
    return 0;
}

int MPIAPI MPI_Finalize(void)
{
    return 0;
}

int MPIAPI MPI_Abort(MPI_Comm, int)
{
    std::cout << "\n>>>>> The fake MPI framework is about to abort.\n" << std::flush;

    throw 0;
}
