#!/bin/bash

# Clean up HPC kernel I/O directories

# Run this SH-script with one argument specifying:
# 1) the flag (0/1) equal ~continuationMode

cd ..

rm "iofiles/host-kernel/terminate" -f
rm "iofiles/host-kernel/snapshot" -f
rm iofiles/kernel-host/iter\ * -f
# Remark: double quotes cannot be used above
rm "iofiles/kernel-host/snapshot/output.mat" -f

if [ $1 == 1 ]; then
    rm "iofiles/host-kernel/input.mat" -f
    rm "iofiles/host-kernel/image.mat" -f
    rm "iofiles/kernel-host/output.mat" -f

    rm "iofiles/kernel-kernel/intermediate.mat" -f

    rm "iofiles/backup/1" -f
    rm "iofiles/backup/2" -f

    rm "iofiles/backup/backup-1/kernel-host/output.mat" -f
    rm "iofiles/backup/backup-1/kernel-kernel/intermediate.mat" -f

    rm "iofiles/backup/backup-2/kernel-host/output.mat" -f
    rm "iofiles/backup/backup-2/kernel-kernel/intermediate.mat" -f
fi