@echo off

rem Initialize variables
call Core\scripts\win-win\params.bat

rem Go to HPC kernel output directory
cd %WORKERDIR%\iofiles\kernel-host

rem Get directory contents using the file name filter specified
dir "iter *" /B