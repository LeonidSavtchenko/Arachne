@echo off

rem Run this BAT-script with one argument specifying the number of backup storage (1/2) to recover from

rem Initialize variables
call Core\scripts\win-lin\params.bat

rem Go to 3rd party software directory containing plink.exe
cd %THIRDPARTYDIR%

rem Recover "output.mat" and "intermediate.mat" (if present)
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% "cd \"%HEADNODEWORKERDIR%/iofiles/backup/backup-%1/kernel-host\"; cp output.mat \"../../../kernel-host/output.mat\" -f"
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% "cd \"%HEADNODEWORKERDIR%/iofiles/backup/backup-%1/kernel-kernel\"; if [ -f intermediate.mat ]; then cp intermediate.mat \"../../../kernel-kernel/intermediate.mat\" -f; else exit 1; fi"