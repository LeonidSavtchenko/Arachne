#!/bin/bash

# Run this SH-script with six or seven arguments specifying:
# 1) the number of processes,
# 2) the number of threads per process,
# 3) the flag (0/1) indicating whether previous simulation session should be continued,
# 4) the number of iterations or the time interval (in ms) to compute in this simulation session,
# 5) the flag (0/1) indicating whether the time interval (0) or the number of iterations (1) is specified,
# 6) list of comma-separated cluster node names to run simulation on,
# 7) (optional) the flag (0/1) indicating whether to disable STDP for this simulation session (image recall mode).

cd ..

# Launch HPC kernel with specified parameters
mpiexec -n $1 --host $6 ./gs.exe $2 $3 $4 $5 $7

if [ $? == 0 ]; then
    touch iofiles/complete
fi