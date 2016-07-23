function astro_alpha_q = astro_alpha_q(v)
    global a2 d1 d2 d3
    
    astro_alpha_q = a2 * d2 * (v + d1) ./ (v + d3);