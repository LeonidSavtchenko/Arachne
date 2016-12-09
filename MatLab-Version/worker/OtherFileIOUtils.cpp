// Do not complain that sprintf and strcpy are used instead of more secure versions sprintf_s and strcpy_s.
// Justification -- Linux compiler does not support sprintf_s and strcpy_s.
#define _CRT_SECURE_NO_WARNINGS

#include "MatFileIO/MatFileIOUtils.h"   // pMat

#include <iostream>     // cout, flush
#include <fstream>      // ifstream, ofstream
#include <cstdio>       // fclose, remove

#include <stdio.h>      // FILE, fopen, remove
#include <string.h>     // strcpy
#include <stdio.h>      // sprintf

#include <mpi.h>        // MPI_Abort


using namespace std;


namespace fileNames
{
    // Remark:
    // Usage of slash "/" instead of backslash "\" below looks odd for Windows,
    // but it's actually correct multiplatform code. (Linux supports only slash while Windows supports both.)
    const char *terminateFile = "./iofiles/host-kernel/terminate\0";
    const char *snapshotFile = "./iofiles/host-kernel/snapshot\0";
    const char *backupStoragePointerFile1 = "./iofiles/backup/1\0";
    const char *backupStoragePointerFile2 = "./iofiles/backup/2\0";
}


bool CheckIfFileExists(const char *path);
void CreateNewEmptyFile(const char *path);


#pragma region Background mode utility methods

// When HPC kernel is running in background mode, this function reports current status
// as a name of the file saved to "kernel-host" directory.
// It also deletes the old file created before.
// (The function is called in master thread of master process only.)
void ReportCurrentProgressBackground(int iter, int m_steps, double iterDur, bool continuationMode, int m_steps_prev)
{
    // Prepare the new file name.
    // Remarks:
    // 1) Linux compiler does not support itoa.
    // 2) Usage of slash "/" instead of backslash "\" below looks odd for Windows,
    //    but it's actually correct multiplatform code. (Linux supports only slash while Windows supports both.)
    static char newFileName[128];
    if (!continuationMode)
    {
        sprintf(newFileName, "./iofiles/kernel-host/iter %i - %i; %g sec", iter + 1, m_steps, iterDur);
    }
    else
    {
        sprintf(newFileName, "./iofiles/kernel-host/iter %i (%i - %i); %g sec", iter + 1, iter + 1 - m_steps_prev, m_steps - m_steps_prev, iterDur);
    }
    
    // Create the new file indicating current status
    CreateNewEmptyFile(newFileName);

    // Delete the old file
    static char oldFileName[128];
    if (oldFileName[0] != '\0')
    {
        int status = remove(oldFileName);
        if (status != 0)
        {
            printf("\n>>>>> Failed to delete the old file \"%s\" indicating previous status.\n", oldFileName);
            cout << flush;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    // The new file name becomes the old one
    strcpy(oldFileName, newFileName);
}


// Check if the file with name "terminate" appeared in "host-kernel" directory.
// If so, soft termination of HPC kernel will start.
// (The function is called in master thread of master process only.)
bool CheckIfTerminationRequested()
{
    return CheckIfFileExists(fileNames::terminateFile);
}


// Check if the file with name "snapshot" appeared in "host-kernel" directory.
// If so, HPC kernel will dump a snapshot immediately.
// (The function is called in master thread of master process only.)
bool CheckIfSnapshotRequested()
{
    return CheckIfFileExists(fileNames::snapshotFile);
}

#pragma endregion

#pragma region Backup file utility methods

// Determine whether 1st or 2nd backup storage should be used for the next backup saving.
// The function is called at start of new simulation session in master thread of master process only.
// Output:
// true  - use 1st backup storage,
// false - use 2nd backup storage.
bool GetTargetBackupStorage(bool continuationMode)
{
    if (!continuationMode)
    {
        // Use 1st backup storage
        return true;
    }
    
    bool firstExists = CheckIfFileExists(fileNames::backupStoragePointerFile1);
    bool secondExists = CheckIfFileExists(fileNames::backupStoragePointerFile2);
    
    if (firstExists && secondExists)
    {
        cout << "\n>>>>> Cannot determine whether 1st or 2nd backup storage should be used for the next backup saving. "
            "Please run Matlab script \"SCRIPT_RecoverBackupProgress.m\" and then restart HPC kernel.\n";
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
        throw;  // Not reached
    }
    else if (secondExists)
    {
        // Use 2nd backup storage
        return false;
    }
    else
    {
        // There is no backup storage pointer file OR there is the file that points to 1st backup storage --
        // the 1st backup storage should be used for the next backup saving
        return true;
    }
}


// The function is called on master thread of master process only
void SwitchTargetBackupStorage(bool switchTo1st)
{
    // Remark:
    // The status returned by "remove" function is not checked intentionally
    // because the file to delete is allowed to be absent
    if (switchTo1st)
    {
        remove(fileNames::backupStoragePointerFile2);
        CreateNewEmptyFile(fileNames::backupStoragePointerFile1);
    }
    else
    {
        remove(fileNames::backupStoragePointerFile1);
        CreateNewEmptyFile(fileNames::backupStoragePointerFile2);
    }
}


// Copy file "output.mat" from the latest backup storage directory
// to the directory "kernel-host/snapshot". Then delete the file "host-kernel/snapshot".
// The method is called on master thread of master process only.
void CopySnapshotFromBackupStorage(bool snapshotFrom1stStorage)
{
    using namespace pMat;
    using namespace fileNames;

    const char *backupOutMatFile = nullptr;

    // Select backup file to copy
    if (snapshotFrom1stStorage)
    {
        backupOutMatFile = backupOutMatFile1;
    }
    else
    {
        backupOutMatFile = backupOutMatFile2;
    }

    // Copy the file
    ifstream src(backupOutMatFile, ios::binary);    // Remark: Linux compiler does not allow syntax
    ofstream dst(snapshotOutMatFile, ios::binary);  //         auto src = ifstream(...); auto dst = ofstream(...);
    dst << src.rdbuf();
    src.close();
    dst.close();

    // Delete file "iofiles/host-kernel/snapshot"
    int status = remove(snapshotFile);
    if (status != 0)
    {
        printf("\n>>>>> Failed to delete the file \"%s\".\n", snapshotFile);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    cout << "Dumping complete.\n" << flush;
}

#pragma endregion

#pragma region Internal utility methods

// Check if specified file exists.
// The function is called in master thread of master process only.
bool CheckIfFileExists(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file != nullptr)
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

// Create the new empty file.
// The function is called in master thread of master process only.
void CreateNewEmptyFile(const char *path)
{
    FILE *file = fopen(path, "w");
    if (file != nullptr)
    {
        fclose(file);
    }
    else
    {
        printf("\n>>>>> Failed to create the new empty file \"%s\".\n", path);
        cout << flush;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
}

#pragma endregion