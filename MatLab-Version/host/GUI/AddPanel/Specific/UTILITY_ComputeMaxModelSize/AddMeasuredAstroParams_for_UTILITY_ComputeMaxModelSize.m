function AddMeasuredAstroParams_for_UTILITY_ComputeMaxModelSize()

    AddPanel('Measured (Astro)');
    
    AddBoolScalar('gatherCaColormap', false, 'Whether to gather data for Ca colormap', 'enableAstro');
    AddIterCounter('caColormapPeriod', 1000, 'How often to get the Ca vector being a column of the colormap (in iterations)', 'enableAstro && gatherCaColormap');
    
end