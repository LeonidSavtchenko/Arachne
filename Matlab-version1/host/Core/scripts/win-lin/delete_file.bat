@echo off

rem Delete file

rem Run this BAT-script with two arguments specifying:
rem 1) the relative path to the file,
rem 2) the name of the file.

rem Initialize variables
call Core\scripts\win-lin\params.bat

rem Go to 3rd party software directory containing plink.exe
cd %THIRDPARTYDIR%

rem Delete the file
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% "rm \"%HEADNODEWORKERDIR%/%1/%2\" -f"