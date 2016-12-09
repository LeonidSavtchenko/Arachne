function AddInitCondAstroParams()

    AddPanel('Init. Cond. (Astro)');

    AddRatVector('Ca', 'ones(num_e, 1) * 0.1', 'mM', 'Calcium concentration', 'enableAstro', 'length(Ca) == num_e');
    AddRatVector('ip3', 'ones(num_e, 1)', 'mM', 'Concentration of IP3', 'enableAstro', 'length(ip3) == num_e');
    AddRatVector('q', 'astro_alpha_q(ip3) ./ (astro_alpha_q(ip3) + astro_beta_q(Ca))', '', 'Kinetic parameter of Ca current', 'enableAstro', 'length(q) == num_e');
    AddRatVector('y', 'ones(num_e, 1) * t_in / tau_spike', '', 'Rate of release of IP3', 'enableAstro', 'length(y) == num_e');
    
end