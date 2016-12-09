#pragma once

#include <mpi.h>
#include "Vector.h"

// Memory allocation mode for LocalVector<T> constructor
enum AllocMode
{
    onlyMasterNew,      // Allocate only on master rank with new T[]
    onlyMasterMxCalloc, // Allocate only on master rank with mxCalloc
                        // (must be used for objects that are going to be saved into MAT-file),
    allRanksNew         // Allocate on all ranks with new T[]
};

template <typename T>
class DistVector;


template <typename T>
class LocalVector : public Vector<T>
{
public:

    // Data
    T *data;

    // Default constructor
    LocalVector();

    // Custom constructor 1: Allocate memory on this rank with new T[]
    LocalVector(int length);
    
    // Custom constructor 2: Allocate memory on master rank or all ranks with new T[] or mxCalloc
    LocalVector(int length, AllocMode allocMode);

    // Custom constructor 3: Bind data on this rank (the memory is not allocated)
    LocalVector(T *data, int length);

    // Inlined operator []

    inline T& operator [] (int idx)
    {
        return data[idx];
    }

    inline const T& operator [] (int idx) const
    {
        return data[idx];
    }

    // Methods
    LocalVector<T>& AssignZeros();
    LocalVector<T>& AssignZeros(int numZeros);
    DistVector<T> Scatter() const;
    void BroadcastData(const LocalVector<T> &data_vector, int startIdx, int dataLength);

    // !! TODO: Investigate why Linux compiler does not allow commented API while MSVS compiler allows
    /*
    template <typename T2>
    LocalVector<T2> CastReshape();
    */
    template <typename T2>
    LocalVector<T2> CastReshape(T2 *typeMarker);

    // Destructor
    ~LocalVector();
};


