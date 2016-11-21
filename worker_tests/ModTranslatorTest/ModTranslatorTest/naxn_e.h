#pragma once 
 
#include "ModCurrentsBase.h" 
 
#include <vector> 
 
namespace mod 
{ 
 
template <typename T> 
class naxn_e : public ModCurrentsBase<T> 
{ 
public: 
 
    naxn_e(); 
 
    // ASSIGNED (public) 
    T v; 
 
    // INITIAL 
    void init(); 
 
    // DERIVATIVE 
    void states(); 
 
    // BREAKPOINT 
    void currents(); 
 
private: 
 
 
    // CONST PARAMETERS 
    const T sh = (T)8; 
    const T gbar = (T)0.010; 
    const T tha = (T)-30; 
    const T qa = (T)7.2; 
    const T Ra = (T)0.4; 
    const T Rb = (T)0.124; 
    const T thi1 = (T)-45; 
    const T thi2 = (T)-45; 
    const T qd = (T)1.5; 
    const T qg = (T)1.5; 
    const T mmin = (T)0.02; 
    const T hmin = (T)0.5; 
    const T q10 = (T)2; 
    const T Rg = (T)0.01; 
    const T Rd = (T).03; 
    const T thinf = (T)-50; 
    const T qinf = (T)4; 
    const T ena = (T)1; 
    const T celsius = (T)1; 
 
    // STATE 
    T m, h; 
 
    // DERIVATIVE 
    T m_rhp, h_rhp; 
 
    // ASSIGNED (private) 
    T ina, thegna, minf, hinf, mtau, htau; 
 
    // PROCEDURE(S) 
    void trates(T vm, T sh2); 
 
    // FUNCTION(S) 
    T trap0(T v, T th, T a, T q); 
 
    // Other 
    const size_t _numCurrents = 1; 
    const size_t _numStates = 2; 
}; 
 
} 
