#pragma once

#include "../ModCurrentsBase.h"

#include <vector>

%DEFINE_PARAMETER

namespace mod
{

template <typename T>
%CLASS_NAME
{
public:

    %PROTOTYPE_DEFAULT_CONSTRUCTOR

    // ASSIGNED (public)

    // INITIAL
    virtual void init() override;

    // DERIVATIVE
    virtual void states() override;

    // BREAKPOINT
    virtual void currents() override;

private:
    // FREE LOCAL VARIABLES
    %FREE_LOCAL_VARIABLES

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
