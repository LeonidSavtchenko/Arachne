#include "stdafx.h" 
 
#include <math.h> 
 
#include "na3n_i.h" 
 
namespace mod 
{ 
 
template <typename T> 
na3n_i<T>::na3n_i() 
{ 
    this->_currents[0] = &ina; 
 
    this->_states = std::vector<T*>(_numStates); 
    this->_states[0] = &m; 
    this->_states[1] = &h; 
    this->_states[2] = &s; 
 
    this->_derivatives = std::vector<T*>(_numStates); 
    this->_derivatives[0] = &m_rhp; 
    this->_derivatives[1] = &h_rhp; 
    this->_derivatives[2] = &s_rhp; 
} 
 
// INITIAL 
template <typename T> 
void na3n_i<T>::init() 
{ 
    trates(v,ar,sh); 
    m=minf; 
    h=hinf; 
    s=sinf; 
} 
 
// DERIVATIVE 
template <typename T> 
void na3n_i<T>::states() 
{ 
    trates(v,ar,sh); 
    m_rhp=(minf-m)/mtau; 
    h_rhp=(hinf-h)/htau; 
    s_rhp=(sinf-s)/taus; 
} 
 
// BREAKPOINT 
template <typename T> 
void na3n_i<T>::currents() 
{ 
    thegna=gbar*m*m*m*h*s; 
    ina=thegna*(v-ena); 
} 
 
// PROCEDURE(S) 
 
template <typename T> 
void na3n_i<T>::trates(T vm, T a2, T sh2) 
{ 
    // LOCAL 
        T  a, b, c, qt; 
 
        qt=pow(q10,((celsius-(T)24)/(T)10)); 
	a = trap0(vm,tha+sh2,Ra,qa); 
	b = trap0(-vm,-tha-sh2,Rb,qa); 
	mtau = (T)1/(a+b)/qt; 
        if (mtau<mmin) {mtau=mmin;} 
	minf = a/(a+b); 
	a = trap0(vm,thi1+sh2,Rd,qd); 
	b = trap0(-vm,-thi2-sh2,Rg,qg); 
	htau =  (T)1/(a+b)/qt; 
        if (htau<hmin) {htau=hmin;} 
	hinf = (T)1/((T)1+exp((vm-thinf-sh2)/qinf)); 
	c=alpv(vm); 
        sinf = c+a2*((T)1-c); 
        taus = bets(vm)/(a0s*((T)1+alps(vm))); 
        if (taus<smax) {taus=smax;} 
} 
 
// FUNCTION(S) 
 
template <typename T> 
T na3n_i<T>::alpv(T v) 
{ 
    T _alpv; 
         _alpv = (T)1/((T)1+exp((v-vvh-sh)/vvs)); 
    return _alpv; 
} 
 
template <typename T> 
T na3n_i<T>::alps(T v) 
{ 
    T _alps; 
  _alps = exp((T)1.e-3*zetas*(v-vhalfs-sh)*(T)9.648e4/((T)8.315*((T)273.16+celsius))); 
    return _alps; 
} 
 
template <typename T> 
T na3n_i<T>::bets(T v) 
{ 
    T _bets; 
  _bets = exp((T)1.e-3*zetas*gms*(v-vhalfs-sh)*(T)9.648e4/((T)8.315*((T)273.16+celsius))); 
    return _bets; 
} 
 
template <typename T> 
T na3n_i<T>::trap0(T v, T th, T a, T q) 
{ 
    T _trap0; 
	if (fabs(v-th) > (T)1e-6) { 
	        _trap0 = a * (v - th) / ((T)1 - exp(-(v - th)/q)); 
	;} else { 
	        _trap0 = a * q; 
 	} 
    return _trap0; 
} 
 
template 
class na3n_i<float>; 
 
template 
class na3n_i<double>; 
 
} 
