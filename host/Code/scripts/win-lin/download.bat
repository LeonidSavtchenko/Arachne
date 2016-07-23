@echo off

rem Run this BAT-script with two arguments:
rem 1) the flag (0/1) indicating whether file "output.mat" should be taken from the snapshot directory,
rem 2) the flag (0/1) indicating whether to compress MAT-file before and decompress after downloading.

rem Initialize variables
call Code\scripts\win-lin\params.bat

rem Go to 3rd party software directory containing plink.exe, pscp.exe and 7za.exe
cd %THIRDPARTYDIR%

if %1 == 1 (
    set "DIR=/snapshot"
) else (
    set "DIR="
)

if %2 == 1 (
    rem Compress file output.mat
    plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% cd \"%HEADNODEWORKERDIR%/iofiles/kernel-host%DIR%\"; rm output.zip -f; zip output.zip output.mat > NUL;

    rem The archive output.zip will be downloaded instead of the file output.mat
    set FILENAME=output.zip
) else (
    rem The file output.mat will be downloaded itself
    set FILENAME=output.mat
)

rem Download output MAT-file or ZIP-file from the head node of HPC cluster
pscp -pw %PASSWORD% %LOGIN%@%HEADNODEIP%:"%HEADNODEWORKERDIR%/iofiles/kernel-host%DIR%/%FILENAME%" "%MATLABHOSTDIR%"

if %2 == 1 (
    rem Go to Matlab host directory.
    rem (Remark: do not remove the quotes!)
    cd "%MATLABHOSTDIR%"

    rem Delete old output MAT-file
    if exist output.mat del output.mat

    rem Unzip just downloaded MAT-file
    "%THIRDPARTYDIR%"\7za.exe e output.zip > NUL

    rem Delete the archive on local machine
    del output.zip

    rem Go to 3rd party software directory
    cd "%THIRDPARTYDIR%"

    rem Delete the archive on remote machine
    plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% cd \"%HEADNODEWORKERDIR%/iofiles/kernel-host%DIR%\"; rm output.zip -f;
)