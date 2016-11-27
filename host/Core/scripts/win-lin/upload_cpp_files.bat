@echo off

rem Initialize variables
call Core\scripts\win-lin\params.bat

rem Go to Matlab temporary directory
cd %1

rem Check if an old archive exists, if so -- delete
if exist AllModCurrents.zip del AllModCurrents.zip

"%THIRDPARTYDIR%\7za.exe" a "%1\AllModCurrents.zip" -r "%1\AllModCurrents.*"

rem Go to 3rd party software directory containing pscp.exe and plink.exe
cd %THIRDPARTYDIR%

rem Upload ZIP-file to the head node of HPC cluster
pscp -pw %PASSWORD% "%1\AllModCurrents.zip" %LOGIN%@%HEADNODEIP%:"%HEADNODEWORKERDIR%/ModCurrents"

rem Unzip "AllModCurrents.*" files and remove archive
plink -pw %PASSWORD% %LOGIN%@%HEADNODEIP% cd \"%HEADNODEWORKERDIR%/ModCurrents\"; rm AllModCurrents.h -f; rm AllModCurrents.cpp -f; unzip AllModCurrents.zip > NUL; rm AllModCurrents.zip -f;

