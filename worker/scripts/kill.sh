#!/bin/bash

# Kill process "gs.exe" running on one of the specified nodes
# This script takes names of nodes as arguments

for var in "$@";
do
    ssh $var "pkill gs.exe" > /dev/null 2>&1
    if [ $? -eq 0 ]
    then
        exit 0 # process is found and killed
    fi
done

exit 1 # process is not running