#pragma once

#include "ModCurrentsBase.h"
#include <vector>

namespace mod 
{ 
 
template <typename T> 
class hh_e : public ModCurrentsBase<T> 
{ 
public: 
 
    hh_e(); 
 
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
 
    // PROCEDURE(S) 
    void rates(T v); 
 
    // FUNCTION(S) 
    T vtrap(T x, T y); 
 
    // Other 
    const size_t _numCurrents = 3; 
    const size_t _numStates = 3; 
}; 
 
} 
namespace mod 
{ 
 
template <typename T> 
class ionleak_e : public ModCurrentsBase<T> 
{ 
public: 
 
    ionleak_e(); 
 
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
    const T gna = (T)1; 
    const T gk = (T)1; 
    const T gca = (T)1; 
 
    // STATE 
 
    // DERIVATIVE 
 
    // ASSIGNED (private) 
    T ena, ek, eca, ina, ik, ica; 
 
    // PROCEDURE(S) 
 
    // FUNCTION(S) 
 
    // Other 
    const size_t _numCurrents = 3; 
}; 
 
} 
namespace mod 
{ 
 
template <typename T> 
class kadist_e : public ModCurrentsBase<T> 
{ 
public: 
 
    kadist_e(); 
 
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
    const T celsius = (T)1; 
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
    const T ek = (T)1; 
 
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
namespace mod 
{ 
 
template <typename T> 
class kdrca1_e : public ModCurrentsBase<T> 
{ 
public: 
 
    kdrca1_e(); 
 
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
namespace mod 
{ 
 
template <typename T> 
class na3n_e : public ModCurrentsBase<T> 
{ 
public: 
 
    na3n_e(); 
 
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
    const T qq = (T)10; 
    const T tq = (T)-55; 
    const T thinf = (T)-50; 
    const T qinf = (T)4; 
    const T vhalfs = (T)-60; 
    const T a0s = (T)0.0003; 
    const T zetas = (T)12; 
    const T gms = (T)0.2; 
    const T smax = (T)10; 
    const T vvh = (T)-58; 
    const T vvs = (T)2; 
    const T ar = (T)1; 
    const T ena = (T)1; 
    const T celsius = (T)1; 
 
    // STATE 
    T m, h, s; 
 
    // DERIVATIVE 
    T m_rhp, h_rhp, s_rhp; 
 
    // ASSIGNED (private) 
    T ina, thegna, minf, hinf, mtau, htau, sinf, taus; 
 
    // PROCEDURE(S) 
    void trates(T vm, T a2, T sh2); 
 
    // FUNCTION(S) 
    T alpv(T v); 
    T alps(T v); 
    T bets(T v); 
    T trap0(T v, T th, T a, T q); 
 
    // Other 
    const size_t _numCurrents = 1; 
    const size_t _numStates = 3; 
}; 
 
} 
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
