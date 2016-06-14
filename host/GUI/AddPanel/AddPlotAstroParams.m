function AddPlotAstroParams()

    AddPanel('Plot (Astro)');
    
    relPred = '~startFromScratch || (enableAstro && ~isempty(watchedAstroIdx))';
    
    AddPlotStyleSelector('Ca', 'calcium', relPred);
    AddPlotStyleSelector('ActParam', 'activation parameter', relPred);
    AddPlotStyleSelector('ReleaseProb', 'release probability', relPred);
    
    AddBoolScalar('plotCaColormap', true, 'Whether to plot Ca colormap', '~startFromScratch || enableAstro');
    
end
