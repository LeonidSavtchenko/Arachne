function AddModelAstroParams()
%% Add astrocyte network parameters

    AddPanel('Model (Astro)');

    AddBoolScalar('enableAstro', false, 'Whether to enable the astrocyte network');
    
    AddPosRatScalar('astroTimeScale', 1000, '', 'Astrocytes evolution time scale divisor', 'enableAstro');
    
    AddRatScalar('v1', 6, 'mV', 'Parameter of astrocyte network', 'enableAstro');
    AddRatScalar('v2', 0.11, 'mV', 'Parameter of astrocyte network', 'enableAstro');
    AddRatScalar('v3', 0.9, 'mV', 'Parameter of astrocyte network', 'enableAstro');
    
    AddRatScalar('d1', 0.13, '然', 'Parameter of astrocyte network', 'enableAstro');
    AddRatScalar('d2', 1.049, '然', 'Parameter of astrocyte network', 'enableAstro');
    AddRatScalar('d3', 0.9434, '然', 'Parameter of astrocyte network', 'enableAstro');
    AddRatScalar('d5', 0.08234, '然', 'Parameter of astrocyte network', 'enableAstro');
    
    AddRatScalar('c1', 0.185, '', 'Parameter of astrocyte network', 'enableAstro');    
    AddRatScalar('a2', 0.2, '1 / 然 / s', 'Parameter of astrocyte network', 'enableAstro');
    AddRatScalar('k3', 0.1, '然', 'Parameter of astrocyte network', 'enableAstro');
    AddRatScalar('ip3star', 0.16, 'mM', 'IP3 basic concnetration', 'enableAstro');
    AddRatScalar('tip3', 7, 'ms', 'IP3 time constant', 'enableAstro');
    AddRatScalar('rip3', 7.2, '1 / ms', 'Parameter of astrocyte network', 'enableAstro');
    
    AddRatScalar('gs', 1, 'mS', 'Parameter of astrocyte network', 'enableAstro');
    
    AddRatScalar('tau_spike', 2, 'ms', 'Parameter of astrocyte network', 'enableAstro');
    AddRatScalar('t_in', 200, 'ms', 'Parameter of astrocyte network', 'enableAstro');
    AddRatScalar('p_basic', 0.2, '', 'Release probability', 'enableAstro');
    AddRatScalar('CaBA', 0.2, 'mM', 'Resting Ca concnetration', 'enableAstro');
    AddRatScalar('Ca_ER', 5, '然', 'Ca_ER concnetration', 'enableAstro');
    
end
    