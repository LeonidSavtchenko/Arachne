@echo off

rem Clean up HPC kernel I/O directories

rem Run this BAT-script with one argument specifying:
rem 1) the flag (0/1) equal ~continuationMode,

rem Initialize variables
call Core\scripts\win-win\params.bat

if not exist %WORKERDIR% goto exit

cd %WORKERDIR%\iofiles\host-kernel
if exist "terminate" del "terminate"
if exist "snapshot" del "snapshot"

cd %WORKERDIR%\iofiles\kernel-host
if exist "iter *" del "iter *"

cd %WORKERDIR%\iofiles\kernel-host\snapshot
if exist "output.mat" del "output.mat"

if "%1" == "1" (
    cd %WORKERDIR%\iofiles\host-kernel
    if exist "input.mat" del "input.mat"
    if exist "image.mat" del "image.mat"

    cd %WORKERDIR%\iofiles\kernel-host
    if exist "output.mat" del "output.mat"

    cd %WORKERDIR%\iofiles\kernel-kernel
    if exist "intermediate.mat" del "intermediate.mat"

    cd %WORKERDIR%\iofiles\backup
    if exist "1" del "1"
    if exist "2" del "2"

    cd %WORKERDIR%\iofiles\backup\backup-1\kernel-host
    if exist "output.mat" del "output.mat"

    cd %WORKERDIR%\iofiles\backup\backup-1\kernel-kernel
    if exist "intermediate.mat" del "intermediate.mat"

    cd %WORKERDIR%\iofiles\backup\backup-2\kernel-host
    if exist "output.mat" del "output.mat"

    cd %WORKERDIR%\iofiles\backup\backup-2\kernel-kernel
    if exist "intermediate.mat" del "intermediate.mat"
)

:exit
cd %WORKERDIR%