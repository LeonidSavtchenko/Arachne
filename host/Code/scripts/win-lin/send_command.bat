@echo off

rem Run this BAT-script with one argument specifying the command to send ("terminate" or "snapshot")

rem Initialize variables
call Code\scripts\win-lin\params.bat

rem Go to 3rd party software directory containing plink.exe
cd %THIRDPARTYDIR%

rem Execute two commands on remote server:
rem 1) go to HPC kernel input directory,
rem 2) create an empty file with name "terminate" or "snapshot" signalling about the request.
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% "cd \"%HEADNODEWORKERDIR%/iofiles/host-kernel\"; touch %1"