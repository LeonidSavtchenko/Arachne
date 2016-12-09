@echo off

rem Initialize variables
call Core\scripts\win-lin\params.bat

rem Go to 3rd party software directory containing plink.exe
cd %THIRDPARTYDIR%

rem Execute two commands on remote server:
rem 1) go to HPC kernel output directory,
rem 2) get directory contents using the file name filter specified
rem    (the "dir" command is called with "-Q" flag to avoid insertion of backslash before each space).
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% "cd \"%HEADNODEWORKERDIR%/iofiles/kernel-host\"; dir iter\ * -Q"