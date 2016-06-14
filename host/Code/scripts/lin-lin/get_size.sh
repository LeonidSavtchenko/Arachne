#!/bin/bash

# Run this SH-script with one argument specifying relative path to the file to get size of

# Initialize variables
. ./Code/scripts/lin-lin/params.sh

# Go to HPC kernel backup directory
cd "$HEADNODEWORKERDIR/iofiles/backup"

# Print size of the file specified by input argument
stat -c %s $1