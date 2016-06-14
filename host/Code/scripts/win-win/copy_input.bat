@echo off

rem Run this BAT-script with one argument specifying:
rem 1) the MAT-file name (without the extension).

rem Initialize variables
call Code\scripts\win-win\params.bat

rem Go to Matlab host directory
cd %MATLABHOSTDIR%

rem Delete old output MAT-file from Matlab host directory (if any)
if exist "output.mat" del "output.mat"

rem Copy the MAT-file from the directory to HPC kernel input directory
xcopy %1.mat %WORKERDIR%\iofiles\host-kernel /Y /Q
