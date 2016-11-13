#pragma once

#include "ModCurrentsBase.h"

#include <vector>

namespace mod
{

template <typename T>
%CLASS_NAME
{
public:

    %PROTOTYPE_DEFAULT_CONSTRUCTOR

    // ASSIGNED (public)
    T v;

    // INITIAL
    void init();

    // DERIVATIVE
    void states();

    // BREAKPOINT
    void currents();

private:

    %CONSTANT_NAMES

    // CONST PARAMETERS
    %PARAMETER_NAMES

    // STATE
    %STATE_NAMES

    // DERIVATIVE
    %DERIVATIVE_NAMES

    // ASSIGNED (private)
    %ASSIGNED_NAMES

    // PROCEDURE(S)
    %PROCEDURE_NAMES

    // FUNCTION(S)
    %FUNCTION_NAMES

    // Other
    %NUM_CURRENTS
    %NUM_STATES
};

}
