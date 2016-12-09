@echo off

rem Run this BAT-script with five or six arguments specifying:
rem 1) the number of processes,
rem 2) the number of threads per process,
rem 3) the flag (0/1) indicating whether previous simulation session should be continued,
rem 4) the number of iterations or the time interval (in ms) to compute in this simulation session,
rem 5) the flag (0/1) indicating whether the time interval (0) or the number of iterations (1) is specified,
rem 6) the flag (0/1) indicating whether to run gs.exe with mpiexec broker or gs_fakeMPI.exe directly,
rem 7) (optional) the flag (0/1) indicating whether disable STDP for this simulation session (image recall mode).

rem Initialize variables
call Core\scripts\win-win\params.bat

rem Go to HPC kernel directory
cd %WORKERDIR%

if %6 == 0 (
    rem Run HPC kernel with specified parameters
    %MPIEXEC% -n %1 gs.exe %2 %3 %4 %5 %7
) else (
    %WORKERDIR%\gs_fakeMPI.exe %2 %3 %4 %5 %7
)

if ERRORLEVEL == 0 (
    rem Create an empty file with name "complete" signalling about successful completion
    copy NUL "iofiles\complete" > NUL
)