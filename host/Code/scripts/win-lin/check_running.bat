@echo off

rem This BAT-script determines whether a process with name "gs.exe" is running on remote cluster
rem This script takes names of nodes as arguments

rem Initialize variables
call Code\scripts\win-lin\params.bat

rem Go to 3rd party software directory containing plink.exe
cd %THIRDPARTYDIR%

rem Check if process "gs.exe" is running on one of the specified nodes
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% sh %REMOTESCRIPTSDIR%/check_running.sh %*