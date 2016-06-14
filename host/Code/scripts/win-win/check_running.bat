@echo off

rem This BAT-script determines if at least one process with name "gs.exe" is running on this machine

tasklist /fi "imagename eq gs.exe" | find ":" > NUL
taskkill /fi "imagename eq gs_fakeMPI.exe" | find ":" > NUL
