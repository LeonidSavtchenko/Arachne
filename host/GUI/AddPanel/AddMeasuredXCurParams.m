function AddMeasuredXCurParams() 

    AddPanel('Measured (X-Cur)');
    
    desc = {'Array of indexes of watched neurons in e-network.'};
    AddPosIntVector('watchedExtraCurrentIdx_e', '[10]', desc, 'enableExtraCurrent_e');
    
    desc = {'Array of indexes of watched neurons in i-network.'};
    AddPosIntVector('watchedExtraCurrentIdx_i', '[10]', desc, 'enableExtraCurrent_i');
end