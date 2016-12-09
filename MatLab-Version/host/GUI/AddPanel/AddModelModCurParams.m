function AddModelModCurParams()

    AddPanel('Model (MOD-Cur)');
    
    %% Section of parameters for e-type neurons
    AddBoolScalar('importMod_e', false, 'Whether to import MOD files with extra currents for e-neurons');
    
    relPred = 'importMod_e';
    AddModFilesSelectorButton('modFiles_e', 'Select MOD file(s)', 'Selector of MOD files with extra currents for e-neurons', relPred);
    
    text = '';
    desc = 'Parameters of the imported currents for e-neurons';
    AddModParamsTextArea('modParams_e', text, desc, relPred);
    
    %% Section of parameters for i-type neurons
    AddBoolScalar('importMod_i', false, 'Whether to import MOD files with extra currents for i-neurons');
    
    relPred = 'importMod_i';
    AddModFilesSelectorButton('modFiles_i', 'Select MOD file(s)', 'Selector of MOD files with extra currents for i-neurons', relPred);
    
    desc = 'Parameters of the imported currents for i-neurons';
    AddModParamsTextArea('modParams_i', text, desc, relPred);

end

