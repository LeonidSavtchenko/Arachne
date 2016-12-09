#pragma once

#include "Containers/LocalDenseMatrix.h"

bool isNaN(float x);
bool isNaN(double x);

template <typename T, typename RNGT>
bool hasNaN(LocalDenseMatrix<T, RNGT> x);
