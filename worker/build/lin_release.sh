#!/bin/bash

MLDIR="/usr/local/MATLAB/R2011b"
GSDIR="/home/savtchenko/amcbridge/phase11/gs/worker"

MLINCLDIR="$MLDIR/extern/include"
MLBINDIR="$MLDIR/bin/glnxa64"
GSINCLDIR="$GSDIR"
CONTAINERS="$GSDIR/Containers"
SCMs="$GSDIR/SCMs"
ASTRO="$GSDIR/Astro"
GABA="$GSDIR/GABA"
STDP="$GSDIR/STDP"
XCUR="$GSDIR/ExtraCurrent"
MATFILEIO="$GSDIR/MatFileIO"

cd ..

mpic++ -fopenmp -std=c++0x -O3 -ffast-math -I$MLINCLDIR -I$GSINCLDIR -L$MLBINDIR -Wl,-rpath=$MLBINDIR -lmat -lmx main.cpp GammaSimulator.cpp DistEnv.cpp DistributionWrapper.cpp BoostedBernoulliDistribution.cpp $MATFILEIO/MatFileIOUtils.cpp MathUtils.cpp OtherFileIOUtils.cpp $CONTAINERS/LocalVector.cpp $CONTAINERS/DistVector.cpp $CONTAINERS/LocalDenseMatrix.cpp $CONTAINERS/LocalSparseMatrix.cpp $CONTAINERS/DistDenseMatrix.cpp $CONTAINERS/DistSparseMatrix.cpp $SCMs/SCMFactory.cpp $SCMs/SCMBase.cpp $SCMs/SimpleSCM.cpp $SCMs/SparseSCM.cpp $SCMs/ZeroSCM.cpp $SCMs/UniformSCM.cpp $SCMs/OneBitSCM.cpp $SCMs/InPlaceSCM.cpp $ASTRO/AstroNet.cpp $GABA/GABA.cpp $STDP/AnalyticStdpMechanism.cpp $STDP/TabulatedStdpMechanism.cpp $STDP/BaseStdpMechanism.cpp $XCUR/ExtraCurrent.cpp -o gs.exe
