#include <stdint.h> // uint8_t, uint64_t

#define _USE_MATH_DEFINES
#include <math.h>   // M_PI

#include "DistEnv.h"
#include "LocalVector.h"
#include "DistVector.h"
#include "GetTypeTagUtils.h"
#include <mpi.h>
#include <matrix.h> // mxCalloc

#include <iostream> // cout, flush
#include <random>   // mt19937, mt19937_64

using namespace std;


// Default constructor
template <typename T>
LocalVector<T>::LocalVector()
{
    this->data = nullptr;
}

// Custom constructor 1: Allocate memory on this rank with new T[]
template <typename T>
LocalVector<T>::LocalVector(int length)
{
    this->data = new T[length];
    this->length = length;
    this->mpiTypeTag = GetMpiDataType<T>();
}

// Custom constructor 2: Allocate memory on master rank or all ranks with new T[] or mxCalloc
template <typename T>
LocalVector<T>::LocalVector(int length, AllocMode allocMode)
{
    using namespace DistEnv;

    switch (allocMode)
    {
        case AllocMode::onlyMasterNew:
            {
                if (myRank == MASTER_RANK)
                {
                    this->data = new T[length];
                    this->length = length;
                }
                else
                {
                    this->data = nullptr;
                    this->length = -1;
                }
                this->mpiTypeTag = GetMpiDataType<T>();
                break;
            }
        case AllocMode::onlyMasterMxCalloc:
            {
                if (myRank == MASTER_RANK)
                {
                    this->data = (T*)mxCalloc(length, sizeof(T));   // mxCalloc initializes data to zero
                    this->length = length;
                }
                else
                {
                    this->data = nullptr;
                    this->length = -1;
                }
                this->mpiTypeTag = GetMpiDataType<T>();
                break;
            }
        case AllocMode::allRanksNew:
            {
                this->data = new T[length];
                this->length = length;
                this->mpiTypeTag = GetMpiDataType<T>();
                break;
            }
        default:
            {
                cout << "\n>>>>> Bad allocMode specified for LocalVector<T> constructor.\n" << flush;
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
    }
}

// Custom constructor 3: Bind data on this rank (the memory is not allocated)
template <typename T>
LocalVector<T>::LocalVector(T *data, int length)
{
    this->data = data;
    this->length = length;
    this->mpiTypeTag = GetMpiDataType<T>();
}

template <typename T>
LocalVector<T>& LocalVector<T>::AssignZeros()
{
    // !! TODO: Use multithreading
    memset(data, 0, sizeof(T) * this->length);
    return *this;
}

template <typename T>
LocalVector<T>& LocalVector<T>::AssignZeros(int numZeros)
{
    // !! TODO: Use multithreading
    memset(data, 0, sizeof(T) * numZeros);
    return *this;
}

// Scatter local vector located on rank 0 into distributed vector located on ranks 0, 1, 2, ..., numRanks - 1
template <typename T>
DistVector<T> LocalVector<T>::Scatter() const
{
    using namespace DistEnv;
    
    // Broadcast local vector length from rank 0 to all other ranks
    int totalLength = this->length;
    MPI_Bcast(&totalLength, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    
    // Allocate memory on each rank
    auto distVector = DistVector<T>(totalLength);
    
    // Get pointer to local data
    T *localData = distVector.localData;
    
    int localLength = distVector.localLength;
    
    if (myRank == MASTER_RANK)
    {
        // Copy own chunk
        memcpy(localData, data, localLength * sizeof(T));
    }
    
    // Scatter data from rank 0 to ranks 1, 2, ..., numRanks - 1
    for (int i = 1; i < numRanks; i++)
    {
        if (myRank == MASTER_RANK)
        {
            // Determine length of chunk to send and start index
            int chunkLength = GetRankChunkLength(totalLength, i);
            int startIdx = GetRankChunkStartIdx(totalLength, i);

            // Send local vector chunk
            MPI_Send(&data[startIdx], chunkLength, this->mpiTypeTag, i, i, MPI_COMM_WORLD);
        }
        else if (myRank == i)
        {
            MPI_Recv(localData, localLength, this->mpiTypeTag, MASTER_RANK, i, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
    }

    return distVector;
}

// Broadcast data from local vector living on master rank to local vector living on all ranks
template <typename T>
void LocalVector<T>::BroadcastData(const LocalVector<T> &data_vector, int startIdx, int dataLength)
{
    using namespace DistEnv;
    
    #pragma omp barrier
    #pragma omp master
    {
        T *data = this->data;
        if (myRank == MASTER_RANK)
        {
            T *data_source = data_vector.data + startIdx;
            memcpy(data, data_source, dataLength * sizeof(T));
        }
        MPI_Bcast(data, dataLength, this->mpiTypeTag, MASTER_RANK, MPI_COMM_WORLD);
    }
    #pragma omp barrier
}

// Create the new local vector that is bound to the same memory region,
// but has other template type and number of elements.
// !! TODO: Investigate why Linux compiler does not allow commented API while MSVS compiler allows
/*
template <typename T>
template <typename T2>
LocalVector<T2> LocalVector<T>::CastReshape()
*/
template <typename T>
template <typename T2>
LocalVector<T2> LocalVector<T>::CastReshape(T2 *typeMarker)
{
    int s1 = sizeof(T);
    int s2 = sizeof(T2);
    int rem = (this->length * s1) % s2;
    if (rem != 0)
    {
        cout << "\n>>>>> Cannot cast and reshape local vector.\n" << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    int newLength = this->length * s1 / s2;

    auto newVector = new LocalVector<T2>;
    newVector->data = (T2*)this->data;
    newVector->length = newLength;

    return *newVector;
}


// !! TODO: Investigate why Linux compiler does not allow commented API while MSVS compiler allows
/*
template
LocalVector<uint8_t> LocalVector<mt19937>::CastReshape<uint8_t>();

template
LocalVector<mt19937> LocalVector<uint8_t>::CastReshape<mt19937>();

template
LocalVector<uint8_t> LocalVector<mt19937_64>::CastReshape<uint8_t>();

template
LocalVector<mt19937_64> LocalVector<uint8_t>::CastReshape<mt19937_64>();
*/

template
LocalVector<uint8_t> LocalVector<mt19937>::CastReshape(uint8_t* typeMarker);

template
LocalVector<mt19937> LocalVector<uint8_t>::CastReshape(mt19937* typeMarker);

template
LocalVector<uint8_t> LocalVector<mt19937_64>::CastReshape(uint8_t* typeMarker);

template
LocalVector<mt19937_64> LocalVector<uint8_t>::CastReshape(mt19937_64* typeMarker);


// Destructor
template <typename T>
LocalVector<T>::~LocalVector()
{
    // Remark: We do not deallocate memory in all container destructors by two reasons.
    // 1) It's necessary to create shallow copies for containers in some cases
    //    (e.g. a copy with shifted data pointer),
    //    but it's not necessary to deallocate memory when the copy is destructed.
    // 2) All new containers (not shallow copies) are created in gamma simulator only before 1st iteration
    //    and used at time of whole simulation session.
    //    The memory is deallocated as a whole by OS when MPI process dies.
    
    // delete[] data;
    data = nullptr;
}


template
class LocalVector<float>;

template
class LocalVector<double>;

template
class LocalVector<bool>;

template
class LocalVector<uint8_t>;

template
class LocalVector<int>;

template
class LocalVector<uint32_t>;

template
class LocalVector<uint64_t>;

template
class LocalVector<mt19937>;

template
class LocalVector<mt19937_64>;