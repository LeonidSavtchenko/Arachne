#include "MatFileIOUtils.h"
#include "GetTypeTagUtils.h"
#include "Containers/LocalVector.h"
#include "Containers/LocalDenseMatrix.h"
#include "Containers/LocalSparseMatrix.h"
#include "DistEnv.h"

#include <stdio.h>  // printf
#include <stdint.h> // uint8_t, uint64_t
#include <mpi.h>

#include <iostream> // cout, flush

using namespace std;


namespace pMat
{
    MATFile *pInMatFile;
    MATFile *pImageMatFile;
    MATFile *pIntermMatFile;
    MATFile *pOutMatFile;

    // Relative paths to input/output/intermediate/backup MAT-files.
    // Remark:
    // Usage of slash "/" instead of backslash "\" below looks odd for Windows,
    // but it's actually correct multiplatform code. (Linux supports only slash while Windows supports both.)
    const char *inMatFile = "./iofiles/host-kernel/input.mat\0";
    const char *imageMatFile = "./iofiles/host-kernel/image.mat\0";
    const char *intermMatFile = "./iofiles/kernel-kernel/intermediate.mat\0";
    const char *outMatFile = "./iofiles/kernel-host/output.mat\0";
    const char *backupIntermMatFile1 = "./iofiles/backup/backup-1/kernel-kernel/intermediate.mat\0";
    const char *backupOutMatFile1 = "./iofiles/backup/backup-1/kernel-host/output.mat\0";
    const char *backupIntermMatFile2 = "./iofiles/backup/backup-2/kernel-kernel/intermediate.mat\0";
    const char *backupOutMatFile2 = "./iofiles/backup/backup-2/kernel-host/output.mat\0";
    const char *snapshotOutMatFile = "./iofiles/kernel-host/snapshot/output.mat\0";
}


// !! TODO: Call mxDestroyArray(aPtr) for scalars and arrays

#pragma region Readers

void ReadCheckArray
(
    const char *name,
    void **arr,
    int numDims,
    int numRows,
    int numCols,
    mxClassID classId,
    bool allowExtraRows = false,
    MatFile matFile = MatFile::Input
)
{
    using namespace pMat;

    // Determine whether file "input.mat", "image.mat", "intermediate.mat" or "output.mat" should be used to read from
    MATFile *pFile = nullptr;
    switch (matFile)
    {
    case MatFile::Input:
        {
            pFile = pInMatFile;
            break;
        }
    case MatFile::Image:
        {
            pFile = pImageMatFile;
            break;
        }
    case MatFile::Intermediate:
        {
            pFile = pIntermMatFile;
            break;
        }
    case MatFile::IntermInput:
        {
            pFile = (pIntermMatFile != nullptr ? pIntermMatFile : pInMatFile);
            break;
        }
    case MatFile::Output:
        {
            pFile = pOutMatFile;
            break;
        }
    default:
        {
            cout << "\n>>>>> Bad matFile argument passed to ReadCheckArray method.\n" << flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    mxArray *arrayPtr = matGetVariable(pFile, name);
    if (arrayPtr == nullptr)
    {
        printf("\n>>>>> mxArray \"%s\" not found.\n", name);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    mxClassID actClassId = mxGetClassID(arrayPtr);
    if (actClassId != classId)
    {
        printf("\n>>>>> mxArray \"%s\" has wrong class: %i.\n", name, actClassId);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    mwSize actNumDims = mxGetNumberOfDimensions(arrayPtr);
    if (actNumDims != numDims)
    {
        printf("\n>>>>> mxArray \"%s\" has wrong number of dimensions: %i.\n", name, (int)actNumDims);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    const mwSize *dims = mxGetDimensions(arrayPtr);

    if (!allowExtraRows && dims[0] != numRows)
    {
        printf("\n>>>>> mxArray \"%s\" has wrong number of rows: %i.\n", name, (int)dims[0]);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    else if (allowExtraRows && dims[0] < (mwSize)numRows)
    {
        printf("\n>>>>> mxArray \"%s\" has too few rows: %i.\n", name, (int)dims[0]);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    if (dims[1] != numCols)
    {
        printf("\n>>>>> mxArray \"%s\" has wrong number of columns: %i.\n", name, (int)dims[1]);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    *arr = mxGetData(arrayPtr);
}

// Given name, read mxArray from MAT-file on master rank,
// validate the array,
// extract scalar value.
template <typename T>
T ReadCheckScalar(const char *name, MatFile matFile)
{
    using namespace DistEnv;

    if (myRank == MASTER_RANK)
    {
        T *scalar = nullptr;
        mxClassID classId = GetMxClassId<T>();
        // Each scalar in Matlab is matrix of size 1-by-1
        ReadCheckArray(name, (void**)&scalar, 2, 1, 1, classId, false, matFile);
        return *scalar;
    }
    else
    {
        return T();
    }
}

// Given name, read mxArray from MAT-file on master rank,
// validate the array,
// extract scalar value,
// broadcast it to all other ranks.
template <typename T>
T ReadCheckBroadcastScalar(const char *name, MatFile matFile)
{
    T scalar = ReadCheckScalar<T>(name, matFile);
    MPI_Datatype dataType = GetMpiDataType<T>();
    MPI_Bcast(&scalar, 1, dataType, MASTER_RANK, MPI_COMM_WORLD);
    return scalar;
}

template <typename T>
T ReadCheckBroadcastScalar(const std::string &name, MatFile matFile)
{
    return ReadCheckBroadcastScalar<T>(name.c_str(), matFile);
}

// Given name, read mxArray from MAT-file "input.mat" or "intermediate.mat" on master rank,
// validate the array,
// construct LocalVector<T> on master rank
template <typename T>
LocalVector<T> ReadCheckVector(const char *name, int minLength, MatFile matFile)
{
    using namespace DistEnv;

    LocalVector<T> vector;
    if (myRank == MASTER_RANK)
    {
        mxClassID classId = GetMxClassId<T>();
        T *data;
        // The vector is stored in Matlab as matrix of size length-by-1
        ReadCheckArray(name, (void**)&data, 2, minLength, 1, classId, true, matFile);
        vector = LocalVector<T>(data, minLength);
    }
    return vector;
}

// Given name, read mxArray from MAT-file "input.mat" or "intermediate.mat" on master rank,
// validate the array,
// construct LocalVector<T> on master rank
template <typename T>
LocalVector<T> ReadCheckFixedSizeVector(const char *name, int length, MatFile matFile)
{
    using namespace DistEnv;

    LocalVector<T> vector;
    if (myRank == MASTER_RANK)
    {
        mxClassID classId = GetMxClassId<T>();
        T *data;
        // The vector is stored in Matlab as matrix of size length-by-1
        ReadCheckArray(name, (void**)&data, 2, length, 1, classId, false, matFile);
        vector = LocalVector<T>(data, length);
    }
    return vector;
}

// Given name, read mxArray from MAT-file "input.mat" or "intermediate.mat" on master rank,
// validate the array,
// construct identical LocalVector<T> on all ranks
template <typename T>
LocalVector<T> ReadCheckBroadcastVector(const char *name, int length, MatFile matFile)
{
    using namespace DistEnv;

    LocalVector<T> vector;
    if (myRank == MASTER_RANK)
    {
        mxClassID classId = GetMxClassId<T>();
        T *data;
        // The vector is stored in Matlab as matrix of size length-by-1
        ReadCheckArray(name, (void**)&data, 2, length, 1, classId, false, matFile);
        vector = LocalVector<T>(data, length);
    }
    else
    {
        // Allocate memory on other ranks
        vector = LocalVector<T>(length);
    }

    // Broadcast data from master rank to all other ranks
    MPI_Bcast(vector.data, length, GetMpiDataType<T>(), MASTER_RANK, MPI_COMM_WORLD);

    return vector;
}

template <typename T>
LocalVector<T> ReadCheckBroadcastVector(const std::string &name, int length, MatFile matFile)
{
    return ReadCheckBroadcastVector<T>(name.c_str(), length, matFile);
}

// Given name, read mxArray from MAT-file "input.mat" or "intermediate.mat" on master rank,
// validate the array,
// construct LocalVector<T> on master rank and scatter it DistVector<T> lying on all ranks.
template <typename T>
DistVector<T> ReadCheckScatterVector(const char *name, int length, MatFile matFile)
{
    LocalVector<T> vector = ReadCheckVector<T>(name, length, matFile);
    return vector.Scatter();
}
// Given name, read mxArray from MAT-file on master rank,
// validate the array,
// copy data to LocalVector<T> on master rank
template <typename T>
void ReadCheckInVector(LocalVector<T> &vector, const char *name, int minLength, MatFile matFile)
{
    using namespace DistEnv;

    if (myRank == MASTER_RANK)
    {
        mxClassID classId = GetMxClassId<T>();
        T *data;
        T *vecData = vector.data;
        // The vector is stored in Matlab as matrix of size minLength-by-1
        ReadCheckArray(name, (void**)&data, 2, minLength, 1, classId, true, matFile);
        memcpy(vecData, data, minLength * sizeof(T));
    }
}

template <typename T>
void ReadCheckInVector(LocalVector<T> &vector, const std::string &name, int minLength, MatFile matFile)
{
    ReadCheckInVector(vector, name.c_str(), minLength, matFile);
}

// Given name, read mxArray from MAT-file on master rank,
// validate the array,
// copy data to LocalDenseMatrix<T, RNGT> on master rank
template <typename T, typename RNGT>
void ReadCheckInMatrix(LocalDenseMatrix<T, RNGT> &matrix, const char *name, int numRows, int numCols, MatFile matFile)
{
    using namespace DistEnv;

    if (myRank == MASTER_RANK)
    {
        mxClassID classId = GetMxClassId<T>();
        T *data;
        T *matData = matrix.data;
        ReadCheckArray(name, (void**)&data, 2, numRows, numCols, classId, false, matFile);
        memcpy(matData, data, numRows * numCols *sizeof(T));
    }
}

template <typename T, typename RNGT>
void ReadCheckInMatrix(LocalDenseMatrix<T, RNGT> &matrix, const std::string &name, int numRows, int numCols, MatFile matFile)
{
    ReadCheckInMatrix(matrix, name.c_str(), numRows, numCols, matFile);
}

// Given name, read mxArray from MAT-file on master rank,
// validate the array,
// copy data LocalDenseMatrix<T, RNGT> on all ranks
template <typename T, typename RNGT>
void ReadCheckBroadcastInMatrix(LocalDenseMatrix<T, RNGT> &matrix, const char *name, int numRows, int numCols, MatFile matFile)
{
    using namespace DistEnv;

    if (myRank == MASTER_RANK)
    {
        mxClassID classId = GetMxClassId<T>();
        T *data;
        T *matData = matrix.data;
        ReadCheckArray(name, (void**)&data, 2, numRows, numCols, classId, false, matFile);
        memcpy(matData, data, numRows * numCols *sizeof(T));
    }
    MPI_Bcast(matrix.data, numRows * numCols, GetMpiDataType<T>(), MASTER_RANK, MPI_COMM_WORLD);
}

// Given name, read mxArray from MAT-file on master rank,
// validate the array,
// construct LocalDenseMatrix<T> on master rank and scatter it to SimpleSCM<T> lying on all ranks.
template <typename T, typename RNGT>
DistDenseMatrix<T, RNGT> ReadCheckScatterMatrix(const char *name, int numRows, int numCols, MatFile matFile)
{
    using namespace DistEnv;

    LocalDenseMatrix<T, RNGT> matrix;
    if (myRank == MASTER_RANK)
    {
        mxClassID classId = GetMxClassId<T>();
        T *data;
        ReadCheckArray(name, (void**)&data, 2, numRows, numCols, classId, false, matFile);
        matrix = LocalDenseMatrix<T, RNGT>(data, numRows, numCols);
    }
    return matrix.Scatter();
}

template <typename T, typename RNGT>
DistSparseMatrix<T, RNGT> ReadCheckScatterSparseMatrix(const char *name, int numRows, int numCols)
{
    using namespace DistEnv;

    LocalSparseMatrix<T, RNGT> matrix;
    if (myRank == MASTER_RANK)
    {
        mxArray *data = matGetVariable(pMat::pInMatFile, name);
        if (data == nullptr)
        {
            printf("mxArray \"%s\" not found.\n", name);
            std::cout << std::flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        if (!mxIsSparse(data))
        {
            printf("mxArray \"%s\" is not sparse.\n", name);
            std::cout << std::flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        if (numRows != mxGetM(data))
        {
            printf("mxArray \"%s\" has wrong number of rows.\n", name);
            std::cout << std::flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        if (numCols != mxGetN(data))
        {
            printf("mxArray \"%s\" has wrong number of columns.\n", name);
            std::cout << std::flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        matrix = LocalSparseMatrix<T, RNGT>(data);
    }
    return matrix.Scatter();
}

// Given name, read mxArray from MAT-file on master rank,
// validate the array,
// construct LocalDenseMatrix<T> on master rank and identical LocalDenseMatrix<T> on all ranks.
template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT> ReadCheckBroadcastMatrix(const char *name, int numRows, int numCols, MatFile matFile)
{
    using namespace DistEnv;

    LocalDenseMatrix<T, RNGT> matrix;
    if (myRank == MASTER_RANK)
    {
        mxClassID classId = GetMxClassId<T>();
        T *data;
        ReadCheckArray(name, (void**)&data, 2, numRows, numCols, classId, false, matFile);
        matrix = LocalDenseMatrix<T, RNGT>(data, numRows, numCols);
    }
    else
    {
        matrix = LocalDenseMatrix<T, RNGT>(numRows, numCols);
    }
    MPI_Bcast(matrix.data, numRows * numCols, GetMpiDataType<T>(), MASTER_RANK, MPI_COMM_WORLD);
    return matrix;
}

template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT> ReadCheckBroadcastMatrix(const std::string &name, int numRows, int numCols, MatFile matFile)
{
    return ReadCheckBroadcastMatrix<T, RNGT>(name.c_str(), numRows, numCols, matFile);
}

// Given name, read mxArray from the input MAT-file on master rank.
// The method must be called with one thread only.
mxArray* ReadMxArray(const char *name)
{
    using namespace DistEnv;
    using namespace pMat;

    if (myRank == MASTER_RANK)
    {
        MATFile *pSrcFile = pInMatFile;

        mxArray *pArray = matGetVariable(pSrcFile, name);

        if (pArray == nullptr)
        {
            printf("\n>>>>> mxArray \"%s\" not found in input MAT-file.\n", name);
            cout << flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        return pArray;
    }
    else
    {
        return nullptr;
    }
}

mxArray* ReadMxArray(const std::string &name)
{
    return ReadMxArray(name.c_str());
}

#pragma endregion

#pragma region Writers

template <typename T>
void WriteScalar(T scalar, const char *name, MatFile matFile)
{
    using namespace pMat;

    // Determine whether file "output.mat" or "intermediate.mat" or should be used to write to
    MATFile *pFile = nullptr;
    switch (matFile)
    {
    case MatFile::Output:
        {
            pFile = pOutMatFile;
            break;
        }
    case MatFile::Intermediate:
        {
            pFile = pIntermMatFile;
            break;
        }
    default:
        {
            cout << "\n>>>>> Bad matFile argument passed to WriteScalar method.\n" << flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    mxClassID classId = GetMxClassId<T>();

    // Each scalar in Matlab is matrix of size 1-by-1
    mxArray *pArray = mxCreateNumericMatrix(1, 1, classId, mxREAL);

    // Matlab requires the scalar to be stored in specially allocated memory
    T *pData = (T*)mxCalloc(1, sizeof(T));
    *pData = scalar;

    mxSetData(pArray, pData);

    int status = matPutVariable(pFile, name, pArray);
    if (status != 0)
    {
        printf("\n>>>>> Failed to put mxArray \"%s\" into MAT-file.\n", name);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    // Free memory allocated for pArray AND memory allocated for pData
    mxDestroyArray(pArray);
}

template <typename T>
void WriteScalar(T scalar, const std::string &name, MatFile matFile)
{
    WriteScalar(scalar, name.c_str(), matFile);
}

template <typename T>
void WriteVector(const LocalVector<T> &vector, const char *name, MatFile matFile)
{
    using namespace pMat;

    // Determine whether file "output.mat" or "intermediate.mat" or should be used to write to
    MATFile *pFile = nullptr;
    switch (matFile)
    {
    case MatFile::Output:
        {
            pFile = pOutMatFile;
            break;
        }
    case MatFile::Intermediate:
        {
            pFile = pIntermMatFile;
            break;
        }
    default:
        {
            cout << "\n>>>>> Bad matFile argument passed to WriteVector method.\n" << flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    mxClassID classId = GetMxClassId<T>();

    mxArray *pArray = mxCreateNumericMatrix(vector.length, 1, classId, mxREAL);

    mxSetData(pArray, vector.data);

    int status = matPutVariable(pFile, name, pArray);
    if (status != 0)
    {
        printf("\n>>>>> Failed to put mxArray \"%s\" into MAT-file.\n", name);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    // Free memory allocated for pArray, but do not free memory allocated for vector.data
    mxSetData(pArray, nullptr);
    mxDestroyArray(pArray);
}

template <typename T>
void WriteVector(const LocalVector<T> &vector, const std::string &name, MatFile matFile)
{
    WriteVector(vector, name.c_str(), matFile);
}

template <typename T, typename RNGT>
void WriteMatrix(const LocalDenseMatrix<T, RNGT> &matrix, const char *name, MatFile matFile)
{
    using namespace pMat;

    // Determine whether file "output.mat" or "intermediate.mat" or should be used to write to
    MATFile *pFile = nullptr;
    switch (matFile)
    {
    case MatFile::Output:
        {
            pFile = pOutMatFile;
            break;
        }
    case MatFile::Intermediate:
        {
            pFile = pIntermMatFile;
            break;
        }
    default:
        {
            cout << "\n>>>>> Bad matFile argument passed to WriteMatrix method.\n" << flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    mxClassID classId = GetMxClassId<T>();

    mxArray *pArray = mxCreateNumericMatrix(matrix.numRows, matrix.numCols, classId, mxREAL);

    mxSetData(pArray, matrix.data);

    int status = matPutVariable(pFile, name, pArray);
    if (status != 0)
    {
        printf("\n>>>>> Failed to put mxArray \"%s\" into MAT-file.\n", name);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    // Free memory allocated for pArray, but do not free memory allocated for vector.data
    mxSetData(pArray, nullptr);
    mxDestroyArray(pArray);
}

// A wrapper of "WriteVector" which cuts the vector to avoid saving uninitialized elements to MAT-file
template <typename T>
void WriteCutVector(LocalVector<T> &vector, const char *name, int currentLength, MatFile matFile)
{
    int totalLength = vector.length;
    vector.length = currentLength;
    WriteVector<T>(vector, name, matFile);
    vector.length = totalLength;
}

// A wrapper of "WriteMatrix" which cuts the matrix to avoid saving uninitialized columns to MAT-file
template <typename T, typename RNGT>
void WriteCutMatrix(LocalDenseMatrix<T, RNGT> &matrix, const char *name, int currentNumCols, MatFile matFile)
{
    int totalNumCols = matrix.numCols;
    matrix.numCols = currentNumCols;
    WriteMatrix<T, RNGT>(matrix, name, matFile);
    matrix.numCols = totalNumCols;
}

template <typename T, typename RNGT>
void WriteCutMatrix(LocalDenseMatrix<T, RNGT> &matrix, const std::string &name, int currentNumCols, MatFile matFile)
{
    WriteCutMatrix(matrix, name.c_str(), currentNumCols, matFile);
}

// Given name, write mxArray to the output MAT-file.
// The method is called just on master thread of master rank.
void WriteMxArray(mxArray *pArray, const char *name)
{
    using namespace pMat;

    MATFile *pDestFile = pOutMatFile;

    int status = matPutVariable(pDestFile, name, pArray);

    if (status != 0)
    {
        printf("\n>>>>> Failed to put mxArray \"%s\" into output MAT-file.\n", name);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
}

void WriteMxArray(mxArray *pArray, const std::string &name)
{
    WriteMxArray(pArray, name.c_str());
}

#pragma endregion

#pragma region Reader instantiations

// ReadCheckScalar

template
double ReadCheckScalar<double>(const char *name, MatFile matFile);

template
int ReadCheckScalar<int>(const char *name, MatFile matFile);

// ReadCheckBroadcastScalar

template
float ReadCheckBroadcastScalar<float>(const char *name, MatFile matFile);

template
double ReadCheckBroadcastScalar<double>(const char *name, MatFile matFile);

template
int ReadCheckBroadcastScalar<int>(const char *name, MatFile matFile);

template
bool ReadCheckBroadcastScalar<bool>(const char *name, MatFile matFile);

template
float ReadCheckBroadcastScalar<float>(const std::string &name, MatFile matFile);

template
double ReadCheckBroadcastScalar<double>(const std::string &name, MatFile matFile);

template
int ReadCheckBroadcastScalar<int>(const std::string &name, MatFile matFile);

// ReadCheckVector

template
LocalVector<float> ReadCheckVector<float>(const char *name, int minLength, MatFile matFile);

template
LocalVector<double> ReadCheckVector<double>(const char *name, int minLength, MatFile matFile);

template
LocalVector<int> ReadCheckVector<int>(const char *name, int minLength, MatFile matFile);

template
LocalVector<uint8_t> ReadCheckVector<uint8_t>(const char *name, int minLength, MatFile matFile);

template
LocalVector<bool> ReadCheckVector<bool>(const char *name, int minLength, MatFile matFile);

// ReadCheckFixedSizeVector

template
LocalVector<bool> ReadCheckFixedSizeVector<bool>(const char *name, int length, MatFile matFile);

// ReadCheckBroadcastVector

template
LocalVector<float> ReadCheckBroadcastVector(const char *name, int length, MatFile matFile);

template
LocalVector<double> ReadCheckBroadcastVector(const char *name, int length, MatFile matFile);

template
LocalVector<int> ReadCheckBroadcastVector(const char *name, int length, MatFile matFile);

template
LocalVector<int> ReadCheckBroadcastVector(const std::string &name, int length, MatFile matFile);

// ReadCheckScatterVector

template
DistVector<float> ReadCheckScatterVector<float>(const char *name, int length, MatFile matFile);

template
DistVector<double> ReadCheckScatterVector<double>(const char *name, int length, MatFile matFile);

template
DistVector<int> ReadCheckScatterVector<int>(const char *name, int length, MatFile matFile);

// ReadCheckInVector

template
void ReadCheckInVector<int>(LocalVector<int> &vector, const char *name, int minLength, MatFile matFile);

template
void ReadCheckInVector<float>(LocalVector<float> &vector, const char *name, int minLength, MatFile matFile);

template
void ReadCheckInVector<double>(LocalVector<double> &vector, const char *name, int minLength, MatFile matFile);

template
void ReadCheckInVector<float>(LocalVector<float> &vector, const std::string &name, int minLength, MatFile matFile);

template
void ReadCheckInVector<double>(LocalVector<double> &vector, const std::string &name, int minLength, MatFile matFile);

// ReadCheckInMatrix

template
void ReadCheckInMatrix<float>(LocalDenseMatrix<float, std::mt19937> &matrix, const char *name, int numRows, int numCols, MatFile matFile);

template
void ReadCheckInMatrix<double>(LocalDenseMatrix<double, std::mt19937> &matrix, const char *name, int numRows, int numCols, MatFile matFile);

template
void ReadCheckInMatrix<double>(LocalDenseMatrix<double, std::mt19937_64> &matrix, const char *name, int numRows, int numCols, MatFile matFile);

template
void ReadCheckInMatrix<bool>(LocalDenseMatrix<bool, std::mt19937> &matrix, const char *name, int numRows, int numCols, MatFile matFile);

template
void ReadCheckInMatrix<bool>(LocalDenseMatrix<bool, std::mt19937_64> &matrix, const char *name, int numRows, int numCols, MatFile matFile);

template
void ReadCheckInMatrix<float>(LocalDenseMatrix<float, std::mt19937> &matrix, const std::string &name, int numRows, int numCols, MatFile matFile);

template
void ReadCheckInMatrix<double>(LocalDenseMatrix<double, std::mt19937> &matrix, const std::string &name, int numRows, int numCols, MatFile matFile);

template
void ReadCheckInMatrix<double>(LocalDenseMatrix<double, std::mt19937_64> &matrix, const std::string &name, int numRows, int numCols, MatFile matFile);

// ReadCheckBroadcastInMatrix

template
void ReadCheckBroadcastInMatrix<float>(LocalDenseMatrix<float, std::mt19937> &matrix, const char *name, int numRows, int numCols, MatFile matFile);

template
void ReadCheckBroadcastInMatrix<double>(LocalDenseMatrix<double, std::mt19937> &matrix, const char *name, int numRows, int numCols, MatFile matFile);

template
void ReadCheckBroadcastInMatrix<double>(LocalDenseMatrix<double, std::mt19937_64> &matrix, const char *name, int numRows, int numCols, MatFile matFile);

// ReadCheckScatterMatrix

template
DistDenseMatrix<float, std::mt19937> ReadCheckScatterMatrix<float>(const char *name, int numRows, int numCols, MatFile matFile);

template
DistDenseMatrix<double, std::mt19937> ReadCheckScatterMatrix<double>(const char *name, int numRows, int numCols, MatFile matFile);

template
DistDenseMatrix<double, std::mt19937_64> ReadCheckScatterMatrix<double>(const char *name, int numRows, int numCols, MatFile matFile);

template
DistDenseMatrix<bool, std::mt19937> ReadCheckScatterMatrix<bool>(const char *name, int numRows, int numCols, MatFile matFile);

template
DistDenseMatrix<bool, std::mt19937_64> ReadCheckScatterMatrix<bool>(const char *name, int numRows, int numCols, MatFile matFile);

// ReadCheckScatterSparseMatrix

template
DistSparseMatrix<float, std::mt19937> ReadCheckScatterSparseMatrix(const char *name, int numRows, int numCols);

template
DistSparseMatrix<double, std::mt19937> ReadCheckScatterSparseMatrix(const char *name, int numRows, int numCols);

template
DistSparseMatrix<double, std::mt19937_64> ReadCheckScatterSparseMatrix(const char *name, int numRows, int numCols);

// ReadCheckBroadcastMatrix

template
LocalDenseMatrix<float, std::mt19937> ReadCheckBroadcastMatrix<float>(const char *name, int numRows, int numCols, MatFile matFile);

template
LocalDenseMatrix<double, std::mt19937> ReadCheckBroadcastMatrix<double>(const char *name, int numRows, int numCols, MatFile matFile);

template
LocalDenseMatrix<double, std::mt19937_64> ReadCheckBroadcastMatrix<double>(const char *name, int numRows, int numCols, MatFile matFile);

template
LocalDenseMatrix<int, std::mt19937> ReadCheckBroadcastMatrix<int>(const char *name, int numRows, int numCols, MatFile matFile);

template
LocalDenseMatrix<int, std::mt19937_64> ReadCheckBroadcastMatrix<int>(const char *name, int numRows, int numCols, MatFile matFile);

template
LocalDenseMatrix<int, std::mt19937> ReadCheckBroadcastMatrix<int>(const std::string &name, int numRows, int numCols, MatFile matFile);

template
LocalDenseMatrix<int, std::mt19937_64> ReadCheckBroadcastMatrix<int>(const std::string &name, int numRows, int numCols, MatFile matFile);

#pragma endregion

#pragma region Writer instantiations

// WriteScalar

template
void WriteScalar<float>(float scalar, const char *name, MatFile matFile);

template
void WriteScalar<double>(double scalar, const char *name, MatFile matFile);

template
void WriteScalar<int>(int scalar, const char *name, MatFile matFile);

template
void WriteScalar<bool>(bool scalar, const char *name, MatFile matFile);

template
void WriteScalar<int>(int scalar, const std::string &name, MatFile matFile);

// WriteVector

template
void WriteVector<float>(const LocalVector<float> &vector, const char *name, MatFile matFile);

template
void WriteVector<double>(const LocalVector<double> &vector, const char *name, MatFile matFile);

template
void WriteVector<int>(const LocalVector<int> &vector, const char *name, MatFile matFile);

template
void WriteVector<uint8_t>(const LocalVector<uint8_t> &vector, const char *name, MatFile matFile);

template
void WriteVector<float>(const LocalVector<float> &vector, const std::string &name, MatFile matFile);

template
void WriteVector<double>(const LocalVector<double> &vector, const std::string &name, MatFile matFile);

// WriteMatrix

template
void WriteMatrix<float, std::mt19937>(const LocalDenseMatrix<float, std::mt19937> &matrix, const char *name, MatFile matFile);

template
void WriteMatrix<double, std::mt19937>(const LocalDenseMatrix<double, std::mt19937> &matrix, const char *name, MatFile matFile);

template
void WriteMatrix<double, std::mt19937_64>(const LocalDenseMatrix<double, std::mt19937_64> &matrix, const char *name, MatFile matFile);

template
void WriteMatrix<bool, std::mt19937>(const LocalDenseMatrix<bool, std::mt19937> &matrix, const char *name, MatFile matFile);

template
void WriteMatrix<bool, std::mt19937_64>(const LocalDenseMatrix<bool, std::mt19937_64> &matrix, const char *name, MatFile matFile);

template
void WriteMatrix<int, std::mt19937>(const LocalDenseMatrix<int, std::mt19937> &matrix, const char *name, MatFile matFile);

template
void WriteMatrix<int, std::mt19937_64>(const LocalDenseMatrix<int, std::mt19937_64> &matrix, const char *name, MatFile matFile);

// WriteCutVector

template
void WriteCutVector<float>(LocalVector<float> &vector, const char *name, int currentLength, MatFile matFile);

template
void WriteCutVector<double>(LocalVector<double> &vector, const char *name, int currentLength, MatFile matFile);

template
void WriteCutVector<int>(LocalVector<int> &vector, const char *name, int currentLength, MatFile matFile);

// WriteCutMatrix

template
void WriteCutMatrix<float, std::mt19937>(LocalDenseMatrix<float, std::mt19937> &matrix, const char *name, int currentNumCols, MatFile matFile);

template
void WriteCutMatrix<double, std::mt19937>(LocalDenseMatrix<double, std::mt19937> &matrix, const char *name, int currentNumCols, MatFile matFile);

template
void WriteCutMatrix<double, std::mt19937_64>(LocalDenseMatrix<double, std::mt19937_64> &matrix, const char *name, int currentNumCols, MatFile matFile);

template
void WriteCutMatrix<float, std::mt19937>(LocalDenseMatrix<float, std::mt19937> &matrix, const std::string &name, int currentNumCols, MatFile matFile);

template
void WriteCutMatrix<double, std::mt19937>(LocalDenseMatrix<double, std::mt19937> &matrix, const std::string &name, int currentNumCols, MatFile matFile);

template
void WriteCutMatrix<double, std::mt19937_64>(LocalDenseMatrix<double, std::mt19937_64> &matrix, const std::string &name, int currentNumCols, MatFile matFile);

#pragma endregion