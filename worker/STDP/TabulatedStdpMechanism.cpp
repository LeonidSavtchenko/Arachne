#include "TabulatedStdpMechanism.h"

template <typename T, typename RNGT>
bool TabulatedStdpMechanism<T, RNGT>::staticVectorsAreRead;

template <typename T, typename RNGT>
LocalVector<T> TabulatedStdpMechanism<T, RNGT>::stdp_factor;

template <typename T, typename RNGT>
LocalVector<int> TabulatedStdpMechanism<T, RNGT>::size_factor;

template <typename T, typename RNGT>
LocalVector<T> TabulatedStdpMechanism<T, RNGT>::t_start_factor;

template <typename T, typename RNGT>
LocalVector<T> TabulatedStdpMechanism<T, RNGT>::t_end_factor;


template
class TabulatedStdpMechanism<float, std::mt19937>;

template
class TabulatedStdpMechanism<double, std::mt19937>;

template
class TabulatedStdpMechanism<double, std::mt19937_64>;
