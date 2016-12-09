#!/bin/bash

# Run this SH-script with one argument specifying relative path to the file to search for

# Initialize variables
. ./Core/scripts/lin-lin/params.sh

# Go to HPC kernel input/output directory
cd "$WORKERDIR"

# Determine whether the file is present
if [ -f $1 ]; then
    exit 0
else
    exit 1
fi