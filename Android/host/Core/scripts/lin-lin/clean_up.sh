#!/bin/bash

# Clean up HPC kernel I/O directories

# Run this SH-script with one argument specifying:
# 1) the flag (0/1) equal ~continuationMode,

# Initialize variables
. ./Core/scripts/lin-lin/params.bat

rm "$WORKERDIR/iofiles/host-kernel/terminate" -f
rm "$WORKERDIR/iofiles/host-kernel/snapshot" -f
rm $WORKERDIR/iofiles/kernel-host/iter\ * -f
# Remark: double quotes cannot be used above
rm "$WORKERDIR/iofiles/kernel-host/snapshot/output.mat" -f

if [ $1 == 1 ]; then
    rm "$WORKERDIR/iofiles/host-kernel/input.mat" -f
    rm "$WORKERDIR/iofiles/kernel-host/output.mat" -f

    rm "$WORKERDIR/iofiles/kernel-kernel/intermediate.mat" -f

    rm "$WORKERDIR/iofiles/backup/1" -f
    rm "$WORKERDIR/iofiles/backup/2" -f

    rm "$WORKERDIR/iofiles/backup/backup-1/kernel-host/output.mat" -f
    rm "$WORKERDIR/iofiles/backup/backup-1/kernel-kernel/intermediate.mat" -f

    rm "$WORKERDIR/iofiles/backup/backup-2/kernel-host/output.mat" -f
    rm "$WORKERDIR/iofiles/backup/backup-2/kernel-kernel/intermediate.mat" -f
fi