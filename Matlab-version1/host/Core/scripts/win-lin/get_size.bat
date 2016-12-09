@echo off

rem Run this BAT-script with one argument specifying relative path to the file to get size of

rem Initialize variables
call Core\scripts\win-lin\params.bat

rem Go to 3rd party software directory containing plink.exe
cd %THIRDPARTYDIR%

rem Execute two commands on remote server:
rem 1) go to HPC kernel backup directory,
rem 2) print size of the file specified by input argument
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% "cd \"%HEADNODEWORKERDIR%/iofiles/backup\"; stat -c %%s %1"