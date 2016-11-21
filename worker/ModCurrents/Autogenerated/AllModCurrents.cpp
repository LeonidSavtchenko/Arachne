#include <math.h> 

#include "AllModCurrents.h"

namespace mod 
{ 
 
template <typename T> 
hh_e<T>::hh_e() 
{ 
    this->_currents[0] = &ina; 
    this->_currents[1] = &ik; 
    this->_currents[2] = &il; 
 
    this->_states = std::vector<T*>(_numStates); 
    this->_states[0] = &m; 
    this->_states[1] = &h; 
    this->_states[2] = &n; 
 
    this->_derivatives = std::vector<T*>(_numStates); 
    this->_derivatives[0] = &m_rhp; 
    this->_derivatives[1] = &h_rhp; 
    this->_derivatives[2] = &n_rhp; 
} 
 
// INITIAL 
template <typename T> 
void hh_e<T>::init() 
{ 
    rates(v); 
    m=minf; 
    h=hinf; 
    n=ninf; 
} 
 
// DERIVATIVE 
template <typename T> 
void hh_e<T>::states() 
{ 
    rates(v); 
    m_rhp=(minf-m)/mtau; 
    h_rhp=(hinf-h)/htau; 
    n_rhp=(ninf-n)/ntau; 
} 
 
// BREAKPOINT 
template <typename T> 
void hh_e<T>::currents() 
{ 
    gna=gnabar*m*m*m*h; 
    ina=gna*(v-ena); 
    gk=gkbar*n*n*n*n; 
    ik=gk*(v-ek); 
    il=gl*(v-el); 
} 
 
// PROCEDURE(S) 
 
template <typename T> 
void hh_e<T>::rates(T v) 
{ 
//Computes rate and other constants at current v. 
                      //Call once from HOC to initialize inf at resting v. 
    // LOCAL 
        T  alpha, beta, sum, q10; 
 
        q10 = pow((T)3,((celsius - (T)6.3)/(T)10)); 
                //"m" sodium activation system 
        alpha = (T).1 * vtrap(-(v+(T)40),(T)10); 
        beta =  (T)4 * exp(-(v+(T)65)/(T)18); 
        sum = alpha + beta; 
	mtau = (T)1/(q10*sum); 
        minf = alpha/sum; 
                //"h" sodium inactivation system 
        alpha = (T).07 * exp(-(v+(T)65)/(T)20); 
        beta = (T)1 / (exp(-(v+(T)35)/(T)10) + (T)1); 
        sum = alpha + beta; 
	htau = (T)1/(q10*sum); 
        hinf = alpha/sum; 
                //"n" potassium activation system 
        alpha = (T).01*vtrap(-(v+(T)55),(T)10) ; 
        beta = (T).125*exp(-(v+(T)65)/(T)80); 
	sum = alpha + beta; 
        ntau = (T)1/(q10*sum); 
        ninf = alpha/sum; 
} 
 
// FUNCTION(S) 
 
template <typename T> 
T hh_e<T>::vtrap(T x, T y) 
{ 
    T _vtrap; 
//Traps for 0 in denominator of rate eqns. 
        if (fabs(x/y) < (T)1e-6) { 
                _vtrap = y*((T)1 - x/y/(T)2); 
        ;}else{ 
                _vtrap = x/(exp(x/y) - (T)1); 
        } 
    return _vtrap; 
} 
 
template 
class hh_e<float>; 
 
template 
class hh_e<double>; 
 
} 
namespace mod 
{ 
 
template <typename T> 
ionleak_e<T>::ionleak_e() 
{ 
    this->_currents[0] = &ina; 
    this->_currents[1] = &ik; 
    this->_currents[2] = &ica; 
 
 
} 
 
// INITIAL 
template <typename T> 
void ionleak_e<T>::init() 
{ 
} 
 
// DERIVATIVE 
template <typename T> 
void ionleak_e<T>::states() 
{ 
} 
 
// BREAKPOINT 
template <typename T> 
void ionleak_e<T>::currents() 
{ 
    ina=gna*(v-ena); 
    ik=gk*(v-ek); 
    ica=gca*(v-eca); 
} 
 
// PROCEDURE(S) 
 
// FUNCTION(S) 
 
template 
class ionleak_e<float>; 
 
template 
class ionleak_e<double>; 
 
} 
namespace mod 
{ 
 
template <typename T> 
kadist_e<T>::kadist_e() 
{ 
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
void kadist_e<T>::init() 
{ 
    rates(v); 
    n=ninf; 
    l=linf; 
} 
 
// DERIVATIVE 
template <typename T> 
void kadist_e<T>::states() 
{ 
    rates(v); 
    n_rhp=(ninf-n)/taun; 
    l_rhp=(linf-l)/taul; 
} 
 
// BREAKPOINT 
template <typename T> 
void kadist_e<T>::currents() 
{ 
    gka=gkabar*n*l; 
    ik=gka*(v-ek); 
} 
 
// PROCEDURE(S) 
 
template <typename T> 
void kadist_e<T>::rates(T v) 
{ 
//callable from hoc 
    // LOCAL 
        T a,qt; 
 
        qt=pow(q10,((celsius-(T)24)/(T)10)); 
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
T kadist_e<T>::alpn(T v) 
{ 
    T _alpn; 
    // LOCAL 
    T zeta; 
 
  zeta=zetan+pw/((T)1+exp((v-tq)/qq)); 
  _alpn = exp((T)1.e-3*zeta*(v-vhalfn)*(T)9.648e4/((T)8.315*((T)273.16+celsius))) ; 
    return _alpn; 
} 
 
template <typename T> 
T kadist_e<T>::betn(T v) 
{ 
    T _betn; 
    // LOCAL 
    T zeta; 
 
  zeta=zetan+pw/((T)1+exp((v-tq)/qq)); 
  _betn = exp((T)1.e-3*zeta*gmn*(v-vhalfn)*(T)9.648e4/((T)8.315*((T)273.16+celsius))) ; 
    return _betn; 
} 
 
template <typename T> 
T kadist_e<T>::alpl(T v) 
{ 
    T _alpl; 
  _alpl = exp((T)1.e-3*zetal*(v-vhalfl)*(T)9.648e4/((T)8.315*((T)273.16+celsius))) ; 
    return _alpl; 
} 
 
template <typename T> 
T kadist_e<T>::betl(T v) 
{ 
    T _betl; 
  _betl = exp((T)1.e-3*zetal*gml*(v-vhalfl)*(T)9.648e4/((T)8.315*((T)273.16+celsius))); 
    return _betl; 
} 
 
template 
class kadist_e<float>; 
 
template 
class kadist_e<double>; 
 
} 
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
namespace mod 
{ 
 
template <typename T> 
na3n_e<T>::na3n_e() 
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
void na3n_e<T>::init() 
{ 
    trates(v,ar,sh); 
    m=minf; 
    h=hinf; 
    s=sinf; 
} 
 
// DERIVATIVE 
template <typename T> 
void na3n_e<T>::states() 
{ 
    trates(v,ar,sh); 
    m_rhp=(minf-m)/mtau; 
    h_rhp=(hinf-h)/htau; 
    s_rhp=(sinf-s)/taus; 
} 
 
// BREAKPOINT 
template <typename T> 
void na3n_e<T>::currents() 
{ 
    thegna=gbar*m*m*m*h*s; 
    ina=thegna*(v-ena); 
} 
 
// PROCEDURE(S) 
 
template <typename T> 
void na3n_e<T>::trates(T vm, T a2, T sh2) 
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
T na3n_e<T>::alpv(T v) 
{ 
    T _alpv; 
         _alpv = (T)1/((T)1+exp((v-vvh-sh)/vvs)); 
    return _alpv; 
} 
 
template <typename T> 
T na3n_e<T>::alps(T v) 
{ 
    T _alps; 
  _alps = exp((T)1.e-3*zetas*(v-vhalfs-sh)*(T)9.648e4/((T)8.315*((T)273.16+celsius))); 
    return _alps; 
} 
 
template <typename T> 
T na3n_e<T>::bets(T v) 
{ 
    T _bets; 
  _bets = exp((T)1.e-3*zetas*gms*(v-vhalfs-sh)*(T)9.648e4/((T)8.315*((T)273.16+celsius))); 
    return _bets; 
} 
 
template <typename T> 
T na3n_e<T>::trap0(T v, T th, T a, T q) 
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
class na3n_e<float>; 
 
template 
class na3n_e<double>; 
 
} 
namespace mod 
{ 
 
template <typename T> 
naxn_e<T>::naxn_e() 
{ 
    this->_currents[0] = &ina; 
 
    this->_states = std::vector<T*>(_numStates); 
    this->_states[0] = &m; 
    this->_states[1] = &h; 
 
    this->_derivatives = std::vector<T*>(_numStates); 
    this->_derivatives[0] = &m_rhp; 
    this->_derivatives[1] = &h_rhp; 
} 
 
// INITIAL 
template <typename T> 
void naxn_e<T>::init() 
{ 
    trates(v,sh); 
    m=minf; 
    h=hinf; 
} 
 
// DERIVATIVE 
template <typename T> 
void naxn_e<T>::states() 
{ 
    trates(v,sh); 
    m_rhp=(minf-m)/mtau; 
    h_rhp=(hinf-h)/htau; 
} 
 
// BREAKPOINT 
template <typename T> 
void naxn_e<T>::currents() 
{ 
    thegna=gbar*m*m*m*h; 
    ina=thegna*(v-ena); 
} 
 
// PROCEDURE(S) 
 
template <typename T> 
void naxn_e<T>::trates(T vm, T sh2) 
{ 
    // LOCAL 
        T  a, b, qt; 
 
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
} 
 
// FUNCTION(S) 
 
template <typename T> 
T naxn_e<T>::trap0(T v, T th, T a, T q) 
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
class naxn_e<float>; 
 
template 
class naxn_e<double>; 
 
} 
