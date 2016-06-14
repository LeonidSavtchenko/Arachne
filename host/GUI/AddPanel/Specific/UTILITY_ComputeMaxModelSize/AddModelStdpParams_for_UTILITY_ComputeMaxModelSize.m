function AddModelStdpParams_for_UTILITY_ComputeMaxModelSize()
%% Add STDP-related parameters
    
    AddPanel('Model (STDP)');
    
    AddBoolScalar('enableSTDP', true, 'Whether to correct matrices of synaptic conductance according to Hebbian theory');
    
end