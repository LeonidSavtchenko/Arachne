#!/bin/bash

# Run this SH-script with seven or nine arguments specifying:
# 1) the number of processes,
# 2) the number of threads per process,
# 3) the flag (0/1) indicating whether previous simulation session should be continued,
# 4) the number of iterations or the time interval (in ms) to compute in this simulation session,
# 5) the flag (0/1) indicating whether the time interval (0) or the number of iterations (1) is specified,
# 6) the flag (0/1) indicating whether the master node idles,
# 7) the flag (0/1) indicating whether HPC kernel should be run in background mode,
# 8) (optional) the flag (0/1) indicating whether to disable STDP for this simulation session (image recall mode),
# 9) (optional) the string of type "00101101..." representing image drives to e-cells.

# Initialize variables
. ./Code/scripts/lin-lin/params.sh

# Go to HPC kernel directory
cd "$WORKERDIR/scripts"

# Run HPC kernel with specified parameters in background or foreground mode depending on 6th input argument
if [ $6 == 1 ]; then
    sh run.sh $1 $2 $3 $4 $5 $7 $8 $9 &
else
    sh run.sh $1 $2 $3 $4 $5 $7 $8 $9
fi