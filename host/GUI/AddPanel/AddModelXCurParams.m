function AddModelXCurParams()
%% Add parameters related to the extra currents
    
    AddPanel('Model (X-Cur)');
    
    %% Section of parameters for e-type neurons
    AddBoolScalar('enableExtraCurrent_e', false, 'Whether to enable the extra current for e-neurons');
    
    relPred = 'enableExtraCurrent_e';
    
    AddRatScalar('xcG_e', 0.1, 'mS', 'The maximum conductance', relPred);
    AddIntScalar('xcP_e', 1, 'The multiplicity of gating elements in the ionic channels', relPred, 'xcP_e >= 0');
    AddIntScalar('xcQ_e', 1, 'The multiplicity of gating elements in the ionic channels', relPred, 'xcQ_e >= 0');
    
    AddRatScalar('xcVr_e', 0, 'mV', 'The reversal potential', relPred);
    
    AddRatScalar('xcVa_e', 0.1, 'mV', 'The threshold of the steady state curve in the a ODE', relPred);    
    AddRatScalar('xcSa_e', 0.1, '1 / mV', 'The slope of the steady state curve in the a ODE', relPred);    
    AddRatScalar('xcTa_e', 1, 'ms', 'The time constant in the a ODE', relPred);    
    
    AddRatScalar('xcVb_e', 0.1, 'mV', 'The threshold of the steady state curve in the b ODE', relPred);    
    AddRatScalar('xcSb_e', 0.1, '1 / mV', 'The slope of the steady state curve in the b ODE', relPred);    
    AddRatScalar('xcTb_e', 1, 'ms', 'The time constant in the b ODE', relPred);
    
    %% Section of parameters for i-type neurons
    AddBoolScalar('enableExtraCurrent_i', false, 'Whether to enable the extra current for i-neurons');
    
    relPred = 'enableExtraCurrent_i';
    
    AddRatScalar('xcG_i', 0.1, 'mS', 'Th maximum conductance', relPred);
    AddIntScalar('xcP_i', 1, 'The multiplicity of gating elements in the ionic channels', relPred, 'xcP_i >= 0');
    AddIntScalar('xcQ_i', 1, 'The multiplicity of gating elements in the ionic channels', relPred, 'xcQ_i >= 0');
    
    AddRatScalar('xcVr_i', 0, 'mV', 'Th reversal potential', relPred);
    
    AddRatScalar('xcVa_i', 0.1, 'mV', 'The threshold of the steady state curve in the a ODE', relPred);    
    AddRatScalar('xcSa_i', 0.1, '1 / mV', 'The slope of the steady state curve in the a ODE', relPred);    
    AddRatScalar('xcTa_i', 1, 'ms', 'The time constant in the a ODE', relPred);    
   
    AddRatScalar('xcVb_i', 0.1, 'mV', 'The threshold of the steady state curve in the b ODE', relPred);    
    AddRatScalar('xcSb_i', 0.1, '1 / mV', 'The slope of the steady state curve in the b ODE', relPred);    
    AddRatScalar('xcTb_i', 1, 'ms', 'The time constant in the b ODE', relPred);
    
end
    