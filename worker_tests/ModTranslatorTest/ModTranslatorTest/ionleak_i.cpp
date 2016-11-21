#include "stdafx.h" 
 
#include <math.h> 
 
#include "ionleak_i.h" 
 
namespace mod 
{ 
 
template <typename T> 
ionleak_i<T>::ionleak_i() 
{ 
    this->_currents[0] = &ina; 
    this->_currents[1] = &ik; 
    this->_currents[2] = &ica; 
 
 
} 
 
// INITIAL 
template <typename T> 
void ionleak_i<T>::init() 
{ 
} 
 
// DERIVATIVE 
template <typename T> 
void ionleak_i<T>::states() 
{ 
} 
 
// BREAKPOINT 
template <typename T> 
void ionleak_i<T>::currents() 
{ 
    ina=gna*(v-ena); 
    ik=gk*(v-ek); 
    ica=gca*(v-eca); 
} 
 
// PROCEDURE(S) 
 
// FUNCTION(S) 
 
template 
class ionleak_i<float>; 
 
template 
class ionleak_i<double>; 
 
} 
