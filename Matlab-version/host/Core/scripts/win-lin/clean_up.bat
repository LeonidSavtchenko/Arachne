@echo off

rem Clean up HPC kernel I/O directories

rem Run this BAT-script with one argument specifying:
rem 1) the flag (0/1) equal ~continuationMode

rem Initialize variables
call Core\scripts\win-lin\params.bat

rem Go to 3rd party software directory containing plink.exe
cd %THIRDPARTYDIR%

rem Execute two commands on remote server:
rem 1) go to HPC kernel directory,
rem 2) run helper SH-script that cleans up HPC kernel I/O directories.
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% "cd \"%REMOTESCRIPTSDIR%\"; sh clean_up.sh %1"