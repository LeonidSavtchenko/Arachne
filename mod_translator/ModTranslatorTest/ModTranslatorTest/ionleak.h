#pragma once 
 
#include "ModCurrentsBase.h" 
 
#include <vector> 
 
namespace mod 
{ 
 
template <typename T> 
class ionleak : public ModCurrentsBase<T> 
{ 
public: 
 
    ionleak(T v, T gna); 
 
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
    T gna; 
 
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
