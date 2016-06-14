function AddPlotStyleSelector(name, descRoot,   relPred, index)

    global plotStyles

    name = ['plot', name, 'Style'];
    
    if nargin < 3
        relPred = 'true';
    end
    
    if nargin < 4
        index = plotStyles.PlotCurvesTogether;
    end
    
    list = fieldnames(plotStyles);
    desc = ['Plot style for ', descRoot, ' curves'];
    descItems = {'Do not plot any curve', ...
                 'Plot each curve separately (on individual figure)', ...
                 'Plot all curves together (on the same figure)'};
              
    AddList(name, list, index, desc, descItems, relPred);

end