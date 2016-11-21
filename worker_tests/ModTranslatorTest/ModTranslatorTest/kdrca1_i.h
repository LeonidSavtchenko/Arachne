#pragma once 
 
#include "ModCurrentsBase.h" 
 
#include <vector> 
 
namespace mod 
{ 
 
template <typename T> 
class kdrca1_i : public ModCurrentsBase<T> 
{ 
public: 
 
    kdrca1_i(); 
 
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
    const T ek = (T)1; 
    const T celsius = (T)1; 
    const T gkdrbar = (T).003; 
    const T vhalfn = (T)13; 
    const T a0n = (T)0.02; 
    const T zetan = (T)-3; 
    const T gmn = (T)0.7; 
    const T nmax = (T)2; 
    const T q10 = (T)1; 
 
    // STATE 
    T n; 
 
    // DERIVATIVE 
    T n_rhp; 
 
    // ASSIGNED (private) 
    T ik, ninf, gkdr, taun; 
 
    // PROCEDURE(S) 
    void rates(T v); 
 
    // FUNCTION(S) 
    T alpn(T v); 
    T betn(T v); 
 
    // Other 
    const size_t _numCurrents = 1; 
    const size_t _numStates = 1; 
}; 
 
} 
