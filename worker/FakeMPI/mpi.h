#pragma once


typedef int MPI_Datatype;

#define MPIAPI __stdcall

typedef int MPI_Comm;
#define MPI_COMM_WORLD ((MPI_Comm)0x44000000)

/* The order of these elements must match that in mpif.h */
typedef struct MPI_Status {
    int count;
    int cancelled;
    int MPI_SOURCE;
    int MPI_TAG;
    int MPI_ERROR;

} MPI_Status;


typedef int MPI_Datatype;
#define MPI_CHAR           ((MPI_Datatype)0x4c000101)
#define MPI_SIGNED_CHAR    ((MPI_Datatype)0x4c000118)
#define MPI_UNSIGNED_CHAR  ((MPI_Datatype)0x4c000102)
#define MPI_BYTE           ((MPI_Datatype)0x4c00010d)
#define MPI_WCHAR          ((MPI_Datatype)0x4c00020e)
#define MPI_SHORT          ((MPI_Datatype)0x4c000203)
#define MPI_UNSIGNED_SHORT ((MPI_Datatype)0x4c000204)
#define MPI_INT            ((MPI_Datatype)0x4c000405)
#define MPI_UNSIGNED       ((MPI_Datatype)0x4c000406)
#define MPI_LONG           ((MPI_Datatype)0x4c000407)
#define MPI_UNSIGNED_LONG  ((MPI_Datatype)0x4c000408)
#define MPI_FLOAT          ((MPI_Datatype)0x4c00040a)
#define MPI_DOUBLE         ((MPI_Datatype)0x4c00080b)
#define MPI_LONG_DOUBLE    ((MPI_Datatype)0x4c00080c)
#define MPI_LONG_LONG_INT  ((MPI_Datatype)0x4c000809)
#define MPI_UNSIGNED_LONG_LONG ((MPI_Datatype)0x4c000819)
#define MPI_LONG_LONG      MPI_LONG_LONG_INT

#define MPI_STATUS_IGNORE (MPI_Status *)1
#define MPI_STATUSES_IGNORE (MPI_Status *)1
#define MPI_ERRCODES_IGNORE (int *)0

/* Collective operations */
typedef int MPI_Op;

#define MPI_SUM     (MPI_Op)(0x58000003)

int MPIAPI MPI_Comm_size(MPI_Comm, int *);

int MPIAPI MPI_Comm_rank(MPI_Comm, int *);

int MPIAPI MPI_Type_contiguous(int, MPI_Datatype, MPI_Datatype *);

int MPIAPI MPI_Type_commit(MPI_Datatype *);

int MPIAPI MPI_Send(void*, int, MPI_Datatype, int, int, MPI_Comm);

int MPIAPI MPI_Recv(void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status *);

int MPIAPI MPI_Barrier(MPI_Comm);

int MPIAPI MPI_Reduce(void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm);

int MPIAPI MPI_Bcast(void*, int, MPI_Datatype, int, MPI_Comm);

int MPIAPI MPI_Init(int *, char ***);

int MPIAPI MPI_Finalize(void);

int MPIAPI MPI_Abort(MPI_Comm, int);

