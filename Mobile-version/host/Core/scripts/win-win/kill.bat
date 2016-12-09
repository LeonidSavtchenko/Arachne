@echo off

rem Terminate all gamma simulator processes forcibly

taskkill /f /im gs.exe > NUL
taskkill /f /im gs_fakeMPI.exe > NUL
