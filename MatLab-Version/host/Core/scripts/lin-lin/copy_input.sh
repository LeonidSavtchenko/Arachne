#!/bin/bash

# Initialize variables
. ./Core/scripts/lin-lin/params.sh

# Go to Matlab host directory
cd "$MATLABHOSTDIR"

# Delete old output MAT-file from Matlab host directory (if any)
rm "output.mat" -f

# Copy input MAT-file from the directory to HPC kernel input directory
cp input.mat "$WORKERDIR/iofiles/host-kernel" -f