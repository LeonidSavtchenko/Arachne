#include "GammaSimulator.h"

// Each CPP-file below contains partial implementation of GammaSimulator class
#include "MatFileIO/ReadInputDataAllocateTemporaryArrays.cpp"
#include "MatFileIO/GatherWriteIntermediateData.cpp"
#include "MatFileIO/WriteOutputData.cpp"
#include "ExternalDrives.cpp"
#include "ElementwiseOp1.cpp"
#include "ElementwiseOp2.cpp"
#include "ElementwiseOp3.cpp"
#include "ElementwiseOp4.cpp"
#include "UpdateIdxTNumSpikes.cpp"
#include "ComputeSynCoef.cpp"
#include "ComputePotObsPts.cpp"
#include "FixCurrentProgress.cpp"
#include "DoSimulation.cpp"


template
class GammaSimulator<float, std::mt19937>;

template
class GammaSimulator<double, std::mt19937>;  

template
class GammaSimulator<double, std::mt19937_64>;
