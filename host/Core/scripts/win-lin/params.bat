@echo off

set HEADNODEIP=0.0.0.0
set LOGIN=my_login
set PASSWORD=my_password
set "HEADNODEWORKERDIR=/home/amcbridge/phase12/worker"

set MATLABHOSTDIR=%cd%
set THIRDPARTYDIR=%MATLABHOSTDIR%\Core\3rdparty

set "REMOTESCRIPTSDIR=%HEADNODEWORKERDIR%/scripts"