#include "stdafx.h"

#include <math.h>

%INCLUDE_HEADER 

namespace mod
{

template <typename T>
%CTOR_SIGNATURE
{
    this->celsius = celsius;

    this->_currents = std::vector<T*>(_numCurrents);
    %INIT_CURRENTS

    this->_states = std::vector<T*>(_numStates);
    %INIT_STATES

    this->_derivatives = std::vector<T*>(_numStates);
    %INIT_DERIVATIVES
}

// INITIAL
template <typename T>
%INITIAL_SIGNATURE
{
    %INITIAL_BODY
}

// DERIVATIVE
template <typename T>
%DERIVATIVE_SIGNATURE
{
    %DERIVATIVE_BODY
}

// BREAKPOINT
template <typename T>
%BREAKPOINT_SIGNATURE
{
    %BREAKPOINT_BODY
}

// PROCEDURE(S)
%PROCEDURE_DEFINITIONS

// FUNCTION(S)
%FUNCTION_DEFINITIONS

template
%TEMPLATE_INSTANTIATION_FLOAT

template
%TEMPLATE_INSTANTIATION_DOUBLE

}
