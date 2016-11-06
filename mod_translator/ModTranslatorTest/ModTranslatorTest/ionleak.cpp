#include "stdafx.h" 
 
#include <math.h> 
 
#include "ionleak.h" 
 
namespace mod 
{ 
 
template <typename T> 
ionleak<T>::ionleak(T v, T gna) 
{ 
    this->celsius = celsius; 
 
    this->_currents = std::vector<T*>(_numCurrents); 
    this->_currents[0] = &ina; 
    this->_currents[1] = &ik; 
    this->_currents[2] = &ica; 
 
    this->_states = std::vector<T*>(_numStates); 
 
    this->_derivatives = std::vector<T*>(_numStates); 
} 
 
// INITIAL 
template <typename T> 
void ionleak<T>::init() 
{ 
} 
 
// DERIVATIVE 
template <typename T> 
void ionleak<T>::states() 
{ 
} 
 
// BREAKPOINT 
template <typename T> 
void ionleak<T>::currents() 
{ 
    ina=gna*(v-ena); 
    ik=gk*(v-ek); 
    ica=gca*(v-eca); 
} 
 
// PROCEDURE(S) 
 
// FUNCTION(S) 
 
template 
class ionleak<float>; 
 
template 
class ionleak<double>; 
 
} 
