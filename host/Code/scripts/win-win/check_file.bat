@echo off

rem Run this BAT-script with one argument specifying relative path to the file to search for

rem Initialize variables
call Code\scripts\win-win\params.bat

rem Go to HPC kernel input/output directory
cd %WORKERDIR%

rem Determine whether the file is present
dir %1 /B > NUL 2<&1