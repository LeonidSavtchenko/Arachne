#pragma once

#include <mat.h>
#include "Containers/DistVector.h"
#include "SCMs/SimpleSCM.h"
#include "SCMs/SparseSCM.h"

// The enum specifying the MAT file to read from or write to
enum MatFile
{
    Input,          // "input.mat"
    Image,          // "image.mat"
    Intermediate,   // "intermediate.mat"
    IntermInput,    // "intermediate.mat" or "input.mat" if the former is absent
    Output          // "output.mat"
};

namespace pMat
{
    extern MATFile *pInMatFile;
    extern MATFile *pImageMatFile;
    extern MATFile *pIntermMatFile;
    extern MATFile *pOutMatFile;

    extern const char *inMatFile;
    extern const char *imageMatFile;
    extern const char *intermMatFile;
    extern const char *outMatFile;
    extern const char *backupIntermMatFile1;
    extern const char *backupOutMatFile1;
    extern const char *backupIntermMatFile2;
    extern const char *backupOutMatFile2;
    extern const char *snapshotOutMatFile;
}

#pragma region Readers

template <typename T>
T ReadCheckScalar(const char *name, MatFile matFile = MatFile::Input);

template <typename T>
T ReadCheckBroadcastScalar(const char *name, MatFile matFile = MatFile::Input);

template <typename T>
T ReadCheckBroadcastScalar(const std::string &name, MatFile matFile = MatFile::Input);

// Load vector from MAT-file only to master rank.
// Extra length is allowed.
template <typename T>
LocalVector<T> ReadCheckVector(const char *name, int minLength, MatFile matFile = MatFile::Input);

// Load vector from MAT-file only to master rank.
// Extra length is not allowed.
template <typename T>
LocalVector<T> ReadCheckFixedSizeVector(const char *name, int length, MatFile matFile = MatFile::Input);

template <typename T>
LocalVector<T> ReadCheckBroadcastVector(const char *name, int length, MatFile matFile = MatFile::Input);

template <typename T>
LocalVector<T> ReadCheckBroadcastVector(const std::string &name, int length, MatFile matFile = MatFile::Input);

template <typename T>
DistVector<T> ReadCheckScatterVector(const char *name, int length, MatFile matFile = MatFile::Input);

template <typename T>
void ReadCheckInVector(LocalVector<T> &vector, const char *name, int minLength, MatFile matFile);

template <typename T>
void ReadCheckInVector(LocalVector<T> &vector, const std::string &name, int minLength, MatFile matFile);

template <typename T, typename RNGT>
void ReadCheckInMatrix(LocalDenseMatrix<T, RNGT> &matrix, const char *name, int numRows, int numCols, MatFile matFile);

template <typename T, typename RNGT>
void ReadCheckInMatrix(LocalDenseMatrix<T, RNGT> &matrix, const std::string &name, int numRows, int numCols, MatFile matFile);

template <typename T, typename RNGT>
void ReadCheckBroadcastInMatrix(LocalDenseMatrix<T, RNGT> &matrix, const char *name, int numRows, int numCols, MatFile matFile);

template <typename T, typename RNGT>
DistDenseMatrix<T, RNGT> ReadCheckScatterMatrix(const char *name, int numRows, int numCols, MatFile matFile = MatFile::Output);

template <typename T, typename RNGT>
DistSparseMatrix<T, RNGT> ReadCheckScatterSparseMatrix(const char *name, int numRows, int numCols);

template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT> ReadCheckBroadcastMatrix(const char *name, int numRows, int numCols, MatFile matFile = MatFile::Input);

template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT> ReadCheckBroadcastMatrix(const std::string &name, int numRows, int numCols, MatFile matFile = MatFile::Input);

mxArray* ReadMxArray(const char *name);

mxArray* ReadMxArray(const std::string &name);

#pragma endregion

#pragma region Writers

template <typename T>
void WriteScalar(T scalar, const char *name, MatFile matFile = MatFile::Output);

template <typename T>
void WriteScalar(T scalar, const std::string &name, MatFile matFile = MatFile::Output);

template <typename T>
void WriteVector(const LocalVector<T> &vector, const char *name, MatFile matFile = MatFile::Output);

template <typename T>
void WriteVector(const LocalVector<T> &vector, const std::string &name, MatFile matFile = MatFile::Output);

template <typename T, typename RNGT>
void WriteMatrix(const LocalDenseMatrix<T, RNGT> &matrix, const char *name, MatFile matFile);

template <typename T>
void WriteCutVector(LocalVector<T> &vector, const char *name, int currentLength, MatFile matFile = MatFile::Output);

template <typename T, typename RNGT>
void WriteCutMatrix(LocalDenseMatrix<T, RNGT> &matrix, const char *name, int currentNumCols, MatFile matFile);

template <typename T, typename RNGT>
void WriteCutMatrix(LocalDenseMatrix<T, RNGT> &matrix, const std::string &name, int currentNumCols, MatFile matFile);

void WriteMxArray(mxArray *pArray, const char *name);

void WriteMxArray(mxArray *pArray, const std::string &name);

#pragma endregion