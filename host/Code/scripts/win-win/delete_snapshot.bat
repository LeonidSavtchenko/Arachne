@echo off

rem Clean up HPC kernel snapshot directory

rem Initialize variables
call Code\scripts\win-win\params.bat

rem Go to HPC kernel snapshot directory
cd %WORKERDIR%\iofiles\kernel-host\snapshot

rem Clean up the directory
del "output.mat"