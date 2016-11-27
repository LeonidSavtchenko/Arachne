#include "ModCurrent.h"
#include "MatFileIO/MatFileIOUtils.h"

#include <omp.h>

// Load data from input MAT-file
template <typename T>
void ModCurrent<T>::ReadInputDataAllocateTemporaryArrays(bool continuationMode, bool enable, int num, const DistVector<T> &v)
{
    // The current itself
    I = DistVector<T>(num);

	//TODO:

}

// Write data to intermediate MAT-file.
// The method is called on all ranks with any number of threads, but not inside "#pragma omp master" region.
template <typename T>
void ModCurrent<T>::GatherWriteIntermediateData()
{
    using namespace DistEnv;

    //TODO:
}


// Read data from the input MAT-file to write in the output MAT-file.
// The method is called just on master rank.
template <typename T>
void ModCurrent<T>::ReadTransitMxArrays()
{
	//TODO:
}


// Write data to output MAT-file.
// The method is called just on master thread of master rank.
template <typename T>
void ModCurrent<T>::WriteOutputData(int num_steps)
{
	//TODO:
}

// Transfer data from the input MAT-file to the output MAT-file.
// The method is called just on master rank.
template <typename T>
void ModCurrent<T>::WriteTransitMxArrays()
{
	//TODO:
}


