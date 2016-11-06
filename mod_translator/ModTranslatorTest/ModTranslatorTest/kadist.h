#pragma once 
 
#include "ModCurrentsBase.h" 
 
#include <vector> 
 
namespace mod 
{ 
 
template <typename T> 
class kadist : public ModCurrentsBase<T> 
{ 
public: 
 
    kadist(T v, T celsius, T ek); 
 
    // ASSIGNED (public) 
    T v; 
 
    // INITIAL 
    void init(); 
 
    // DERIVATIVE 
    void states(); 
 
    // BREAKPOINT 
    void currents(); 
 
private: 
 
    // PARAMETER 
    T celsius; 
    const T gkabar = (T).008; 
    const T vhalfn = (T)-1; 
    const T vhalfl = (T)-56; 
    const T a0l = (T)0.05; 
    const T a0n = (T).1; 
    const T zetan = (T)-1.8; 
    const T zetal = (T)3; 
    const T gmn = (T)0.39; 
    const T gml = (T)1; 
    const T lmin = (T)2; 
    const T nmin = (T)0.2; 
    const T pw = (T)-1; 
    const T tq = (T)-40; 
    const T qq = (T)5; 
    const T q10 = (T)5; 
    const T qtl = (T)1; 
    T ek; 
 
    // STATE 
    T n, l; 
 
    // DERIVATIVE 
    T n_rhp, l_rhp; 
 
    // ASSIGNED (private) 
    T ik, ninf, linf, taul, taun, gka; 
 
    // PROCEDURE(S) 
    void rates(T v); 
 
    // FUNCTION(S) 
    T alpn(T v); 
    T betn(T v); 
    T alpl(T v); 
    T betl(T v); 
 
    // Other 
    const size_t _numCurrents = 1; 
    const size_t _numStates = 2; 
}; 
 
} 
