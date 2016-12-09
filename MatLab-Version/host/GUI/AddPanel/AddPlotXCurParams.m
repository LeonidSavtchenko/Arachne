function AddPlotXCurParams()

    AddPanel('Plot (X-Cur)');
    
    relPred = '~startFromScratch || (enableExtraCurrent_e && ~isempty(watchedExtraCurrentIdx_e))';
    AddPlotStyleSelector('I_e', 'Extra current for e-neurons', relPred);
   
    relPred = '~startFromScratch || (enableExtraCurrent_i && ~isempty(watchedExtraCurrentIdx_i))';
    AddPlotStyleSelector('I_i', 'Extra current for i-neurons', relPred);
    
end

