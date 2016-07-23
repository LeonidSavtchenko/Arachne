@echo off

rem Delete file

rem Run this BAT-script with two arguments specifying:
rem 1) the relative path to the file,
rem 2) the name of the file.

rem Initialize variables
call Code\scripts\win-win\params.bat

rem Go to the specified directory
cd %WORKERDIR%\%1

rem Delete the file
del %2