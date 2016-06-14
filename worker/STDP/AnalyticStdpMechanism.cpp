#include "AnalyticStdpMechanism.h"

template <typename T, typename RNGT>
LocalDenseMatrix<T, RNGT> AnalyticStdpMechanism<T, RNGT>::stdpParams;


template
class AnalyticStdpMechanism<float, std::mt19937>;

template
class AnalyticStdpMechanism<double, std::mt19937>;

template
class AnalyticStdpMechanism<double, std::mt19937_64>;
