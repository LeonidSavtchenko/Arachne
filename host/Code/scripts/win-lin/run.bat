@echo off

rem Run this BAT-script with six or seven arguments specifying:
rem 1) the number of processes,
rem 2) the number of threads per process,
rem 3) the flag (0/1) indicating whether previous simulation session should be continued,
rem 4) the number of iterations or the time interval (in ms) to compute in this simulation session,
rem 5) the flag (0/1) indicating whether the time interval (0) or the number of iterations (1) is specified,
rem 6) list of comma-separated cluster node names to run simulation on,
rem 7) (optional) the flag (0/1) indicating whether to disable STDP for this simulation session (image recall mode).

rem Initialize variables
call Code\scripts\win-lin\params.bat

rem Go to 3rd party software directory containing plink.exe
cd %THIRDPARTYDIR%

rem Execute two commands on remote server:
rem 1) go to HPC kernel directory,
rem 2) run helper SH-script in background mode.
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% "cd \"%REMOTESCRIPTSDIR%\"; sh run.sh %1 %2 %3 %4 %5 %6 %7"