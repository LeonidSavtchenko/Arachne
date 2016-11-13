#include "stdafx.h" 
 
#include <math.h> 
 
#include "kdrca1_e.h" 
 
namespace mod 
{ 
 
template <typename T> 
kdrca1_e<T>::kdrca1_e() 
{ 
    this->_currents[0] = &ik; 
 
    this->_states = std::vector<T*>(_numStates); 
    this->_states[0] = &n; 
 
    this->_derivatives = std::vector<T*>(_numStates); 
    this->_derivatives[0] = &n_rhp; 
} 
 
// INITIAL 
template <typename T> 
void kdrca1_e<T>::init() 
{ 
    rates(v); 
    n=ninf; 
} 
 
// DERIVATIVE 
template <typename T> 
void kdrca1_e<T>::states() 
{ 
    rates(v); 
    n_rhp=(ninf-n)/taun; 
} 
 
// BREAKPOINT 
template <typename T> 
void kdrca1_e<T>::currents() 
{ 
    gkdr=gkdrbar*n; 
    ik=gkdr*(v-ek); 
} 
 
// PROCEDURE(S) 
 
template <typename T> 
void kdrca1_e<T>::rates(T v) 
{ 
//callable from hoc 
    // LOCAL 
        T a,qt; 
 
        qt=pow(q10,((celsius-(T)24)/(T)10)); 
        a = alpn(v); 
        ninf = (T)1/((T)1+a); 
        taun = betn(v)/(qt*a0n*((T)1+a)); 
	if (taun<nmax) {taun=nmax;} 
} 
 
// FUNCTION(S) 
 
template <typename T> 
T kdrca1_e<T>::alpn(T v) 
{ 
    T _alpn; 
  _alpn = exp((T)1.e-3*zetan*(v-vhalfn)*(T)9.648e4/((T)8.315*((T)273.16+celsius))) ; 
    return _alpn; 
} 
 
template <typename T> 
T kdrca1_e<T>::betn(T v) 
{ 
    T _betn; 
  _betn = exp((T)1.e-3*zetan*gmn*(v-vhalfn)*(T)9.648e4/((T)8.315*((T)273.16+celsius))) ; 
    return _betn; 
} 
 
template 
class kdrca1_e<float>; 
 
template 
class kdrca1_e<double>; 
 
} 
