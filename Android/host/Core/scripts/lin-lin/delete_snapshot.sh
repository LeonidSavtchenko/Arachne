#!/bin/bash

# Clean up HPC kernel snapshot directory

# Initialize variables
. ./Core/scripts/lin-lin/params.bat

# Clean up the directory
rm "$WORKERDIR/iofiles/kernel-host/snapshot/output.mat" -f