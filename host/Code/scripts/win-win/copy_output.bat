@echo off

rem Run this BAT-script with one Boolean argument (0/1) to specify whether file "output.mat" should be copied from the snapshot directory

rem Initialize variables
call Code\scripts\win-win\params.bat

if %1 == 1 (
    set "DIR=\snapshot"
) else (
    set "DIR="
)

rem Go to HPC kernel output directory
cd %WORKERDIR%\iofiles\kernel-host%DIR%

rem Copy output MAT-file from HPC kernel output directory to Matlab host directory
xcopy output.mat %MATLABHOSTDIR% /Y /Q