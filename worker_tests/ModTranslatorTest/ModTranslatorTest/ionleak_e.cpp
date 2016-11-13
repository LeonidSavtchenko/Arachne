#include "stdafx.h" 
 
#include <math.h> 
 
#include "ionleak_e.h" 
 
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
