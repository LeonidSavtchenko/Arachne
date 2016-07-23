@echo off

rem Run this BAT-script with one argument:
rem 1) the flag (0/1) indicating whether to compress MAT-file before and decompress after uploading,
rem 2) the MAT-file name (without the extension).

rem Initialize variables
call Code\scripts\win-lin\params.bat

set SOURCEFILE=%2

rem Go to Matlab host directory
cd %MATLABHOSTDIR%

if %1 == 1 (
    rem Check if an old archive exists, if so -- delete
    if exist %SOURCEFILE%.zip del %SOURCEFILE%.zip

    rem Compress MAT-file
    "%THIRDPARTYDIR%\7za.exe" a %SOURCEFILE%.zip %SOURCEFILE%.mat > NUL

    rem The ZIP-archive will be uploaded instead of the MAT-file
    set FILENAME=%SOURCEFILE%.zip
) else (
    rem The MAT-file will be uploaded itself
    set FILENAME=%SOURCEFILE%.mat
)

rem Go to 3rd party software directory containing pscp.exe and plink.exe
cd %THIRDPARTYDIR%

rem Upload MAT-file or ZIP-file to the head node of HPC cluster
pscp -pw %PASSWORD% "%MATLABHOSTDIR%\%FILENAME%" %LOGIN%@%HEADNODEIP%:"%HEADNODEWORKERDIR%/iofiles/host-kernel"

if %1 == 1 (
    rem Unzip MAT-file and remove archive
    plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% cd \"%HEADNODEWORKERDIR%/iofiles/host-kernel\"; rm %SOURCEFILE%.mat -f; unzip %SOURCEFILE%.zip > NUL; rm %SOURCEFILE%.zip -f;

    rem Go to Matlab host directory.
    rem (Remark: do not remove the quotes!)
    cd "%MATLABHOSTDIR%"

    rem Delete just uploaded archive
    del %SOURCEFILE%.zip
)