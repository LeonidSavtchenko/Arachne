@echo off

rem Run this BAT-script with one argument specifying the number of backup storage (1/2) to recover from

rem Initialize variables
call Core\scripts\win-win\params.bat

rem Recover "output.mat"
cd %WORKERDIR%\iofiles\backup\backup-%1\kernel-host
xcopy output.mat %WORKERDIR%\iofiles\kernel-host /Y /Q

rem Recover "intermediate.mat" (if present)
cd %WORKERDIR%\iofiles\backup\backup-%1\kernel-kernel
if exist intermediate.mat (
    xcopy intermediate.mat %WORKERDIR%\iofiles\kernel-kernel /Y /Q
    exit /b 0
) else (
    exit /b 1
)