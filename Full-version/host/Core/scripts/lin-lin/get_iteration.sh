#!/bin/bash

# Initialize variables
. ./Core/scripts/lin-lin/params.sh;

# Go to HPC kernel output directory
cd "$WORKERDIR/iofiles/kernel-host";

# Get directory contents using the file name filter specified
dir iter\ * -Q;