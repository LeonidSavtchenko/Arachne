@echo off

rem Run this BAT-script with one argument specifying the command to send ("terminate" or "snapshot")

rem Initialize variables
call Code\scripts\win-win\params.bat

rem Go to HPC kernel input directory
cd %WORKERDIR%\iofiles\host-kernel

rem Create an empty file with name "terminate" or "snapshot" signalling about the request
copy NUL "%1" > NUL