#pragma once 
 
#include "ModCurrentsBase.h" 
 
#include <vector> 
 
namespace mod 
{ 
 
template <typename T> 
class ionleak_i : public ModCurrentsBase<T> 
{ 
public: 
 
    ionleak_i(); 
 
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
