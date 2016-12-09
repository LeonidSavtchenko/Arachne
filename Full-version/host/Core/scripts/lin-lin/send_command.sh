#!/bin/bash

# Run this SH-script with one argument specifying the command to send ("terminate" or "snapshot")

# Initialize variables
. ./Core/scripts/lin-lin/params.sh

# Go to HPC kernel input directory
cd "$WORKERDIR/iofiles/host-kernel"

# Create an empty file with name "terminate" or "snapshot" signalling about the request
touch $1