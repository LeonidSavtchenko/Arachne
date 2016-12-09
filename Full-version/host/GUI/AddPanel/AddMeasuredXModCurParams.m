function AddMeasuredXModCurParams() 

    AddPanel('Meas. (X/MOD-Cur)');
    
    desc = {'Array of indexes of watched neurons in e-network.'};
    AddPosIntVector('watchedExtraCurrentIdx_e', '[10]', desc, 'enableExtraCurrent_e');
    
    desc = {'Array of indexes of watched neurons in i-network.'};
    AddPosIntVector('watchedExtraCurrentIdx_i', '[10]', desc, 'enableExtraCurrent_i');
    
    desc = {'Array of indexes of watched neurons in e-network.'};
    AddPosIntVector('watchedModCurrentIdx_e', '[10]', desc, 'importMod_e');
    
    desc = {'Array of indexes of watched neurons in i-network.'};
    AddPosIntVector('watchedModCurrentIdx_i', '[10]', desc, 'importMod_i');

end