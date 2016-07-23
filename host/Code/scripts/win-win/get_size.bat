@echo off

rem Run this BAT-script with one argument specifying relative path to the file to get size of

rem Initialize variables
call Code\scripts\win-win\params.bat

rem Go to HPC kernel backup directory
cd %WORKERDIR%\iofiles\backup

rem Print size of the file specified by input argument
echo %~z1