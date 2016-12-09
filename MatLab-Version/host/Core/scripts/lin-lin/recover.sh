#!/bin/bash

# Run this SH-script with one argument specifying the number of backup storage (1/2) to recover from

# Initialize variables
. ./Core/scripts/lin-lin/params.sh

# Recover "output.mat"
cd "$WORKERDIR/iofiles/backup/backup-$1/kernel-host"
cp output.mat "../../../kernel-host/output.mat" -f

# Recover "intermediate.mat" (if present)
cd "$WORKERDIR/iofiles/backup/backup-$1/kernel-kernel"
if [ -f intermediate.mat ]; then
    cp intermediate.mat "../../../kernel-kernel/intermediate.mat" -f
else
    exit 1
fi