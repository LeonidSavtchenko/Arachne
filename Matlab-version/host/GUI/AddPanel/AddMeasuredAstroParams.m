function AddMeasuredAstroParams()

    AddPanel('Measured (Astro)');
    
    desc = {'Array of indexes of watched cells that specifies:', ...
            '1) astrocytes to watch Ca of (the curves of Ca vs time will be shown);', ...
            '2) astrocytes to watch the activation parameter "y" of;', ...
            '3) e-neurons to watch the release probability "e->i" of (the curves of releaseProb_ei vs time will be shown).', ...
            'The array can be empty.'};
    AddPosIntVector('watchedAstroIdx', '[55]', desc, 'enableAstro');
    
    AddBoolScalar('gatherCaColormap', false, 'Whether to gather data for Ca colormap', 'enableAstro');
    AddIterCounter('caColormapPeriod', 1000, 'How often to get the Ca vector being a column of the colormap (in iterations)', 'enableAstro && gatherCaColormap');
    
end