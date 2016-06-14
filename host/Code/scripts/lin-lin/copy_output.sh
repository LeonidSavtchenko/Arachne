#!/bin/bash

# Run this SH-script with one Boolean argument (0/1) to specify whether file "output.mat" should be copied from the snapshot directory

# Initialize variables
. ./Code/scripts/lin-lin/params.sh

if [ $1 == 1 ]; then
    DIR=kernel-host/snapshot
else
    DIR=kernel-host
fi

# Go to HPC kernel output directory
cd "$WORKERDIR/iofiles/$DIR"

# Copy output MAT-file from the directory to Matlab host directory
cp output.mat "$MATLABHOSTDIR" -f