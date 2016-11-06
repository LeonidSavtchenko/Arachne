#include "stdafx.h" 
 
#include <math.h> 
 
#include "kadist.h" 
 
namespace mod 
{ 
 
template <typename T> 
kadist<T>::kadist(T v, T celsius, T ek) 
{ 
    this->celsius = celsius; 
 
    this->_currents = std::vector<T*>(_numCurrents); 
    this->_currents[0] = &ik; 
 
    this->_states = std::vector<T*>(_numStates); 
    this->_states[0] = &n; 
    this->_states[1] = &l; 
 
    this->_derivatives = std::vector<T*>(_numStates); 
    this->_derivatives[0] = &n_rhp; 
    this->_derivatives[1] = &l_rhp; 
} 
 
// INITIAL 
template <typename T> 
void kadist<T>::init() 
{ 
    rates(v); 
    n=ninf; 
    l=linf; 
} 
 
// DERIVATIVE 
template <typename T> 
void kadist<T>::states() 
{ 
    rates(v); 
    n_rhp=(ninf-n)/taun; 
    l_rhp=(linf-l)/taul; 
} 
 
// BREAKPOINT 
template <typename T> 
void kadist<T>::currents() 
{ 
    gka=gkabar*n*l; 
    ik=gka*(v-ek); 
} 
 
// PROCEDURE(S) 
 
template <typename T> 
void kadist<T>::rates(T v) 
{ 
//callable from hoc 
    // LOCAL 
        T a,qt; 
 
        qt=q10*((celsius-(T)24)/(T)10); 
        a = alpn(v); 
        ninf = (T)1/((T)1 + a); 
        taun = betn(v)/(qt*a0n*((T)1+a)); 
        if (taun<nmin) {taun=nmin;} 
        a = alpl(v); 
        linf = (T)1/((T)1+ a); 
        taul = (T)0.26*(v+(T)50)/qtl; 
        if (taul<lmin/qtl) {taul=lmin/qtl;} 
} 
 
// FUNCTION(S) 
 
template <typename T> 
T kadist<T>::alpn(T v) 
{ 
    T _alpn; 
  zeta=zetan+pw/((T)1+exp((v-tq)/qq)); 
  _alpn = exp((T)1.e-3*zeta*(v-vhalfn)*(T)9.648e4/((T)8.315*((T)273.16+celsius))) ; 
    return _alpn; 
} 
 
template <typename T> 
T kadist<T>::betn(T v) 
{ 
    T _betn; 
  zeta=zetan+pw/((T)1+exp((v-tq)/qq)); 
  _betn = exp((T)1.e-3*zeta*gmn*(v-vhalfn)*(T)9.648e4/((T)8.315*((T)273.16+celsius))) ; 
    return _betn; 
} 
 
template <typename T> 
T kadist<T>::alpl(T v) 
{ 
    T _alpl; 
  _alpl = exp((T)1.e-3*zetal*(v-vhalfl)*(T)9.648e4/((T)8.315*((T)273.16+celsius))) ; 
    return _alpl; 
} 
 
template <typename T> 
T kadist<T>::betl(T v) 
{ 
    T _betl; 
  _betl = exp((T)1.e-3*zetal*gml*(v-vhalfl)*(T)9.648e4/((T)8.315*((T)273.16+celsius))); 
    return _betl; 
} 
 
template 
class kadist<float>; 
 
template 
class kadist<double>; 
 
} 
