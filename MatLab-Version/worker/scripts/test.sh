#!/bin/bash

# Run this SH-script with two arguments specifying:
# 1) the number of processes,
# 2) list of comma-separated cluster node names to run simulation on.

cd ..

# Launch HPC kernel with specified parameters
mpiexec -n $1 --host $2 ./gs.exe 2> /dev/null