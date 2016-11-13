#pragma once

#include "ModCurrentsBase.h"

#include <vector>

template <typename T>
class ModCurrents : public ModCurrentsBase<T>
{
public:
    ModCurrents(T celsius);

    // ASSIGNED (public)
    T v;

    // INITIAL
    void init();

    // DERIVATIVE
    void states();

    // BREAKPOINT
    void currents();

private:
    const T gnabar = (T).12;
    const T gkbar = (T).036;
    const T gl = (T).0003;
    const T el = (T)-54.3;

    // STATE
    T m, h, n;

    // DERIVATIVE
    T m_rhp, h_rhp, n_rhp;

    // ASSIGNED (private)
    T celsius, ena, ek, gna, gk, ina, ik, il, minf, hinf, ninf, mtau, htau, ntau;

    // PROCEDURE
    void rates(T v);

    // FUNCTION
    T vtrap(T x, T y);

    // Other
    const size_t _numCurrents = 3;
    const size_t _numStates = 3;
};
