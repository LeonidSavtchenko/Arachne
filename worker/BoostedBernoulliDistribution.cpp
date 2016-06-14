#include "BoostedBernoulliDistribution.h"

template
class BoostedBernoulliDistribution<float, std::mt19937>;

template
class BoostedBernoulliDistribution<double, std::mt19937>;

template
class BoostedBernoulliDistribution<double, std::mt19937_64>;