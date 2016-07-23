@echo off

rem Clean up HPC kernel snapshot directory

rem Initialize variables
call Code\scripts\win-lin\params.bat

rem Go to 3rd party software directory containing plink.exe
cd %THIRDPARTYDIR%

rem Clean up HPC kernel snapshot directory
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% "rm \"%HEADNODEWORKERDIR%/iofiles/kernel-host/snapshot/output.mat\" -f"