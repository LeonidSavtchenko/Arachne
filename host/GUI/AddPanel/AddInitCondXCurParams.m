function AddInitCondXCurParams()
%% Initial conditions for the extra currents ODEs

    AddPanel('Init. Cond. (X-Cur)');

    relPred = 'enableExtraCurrent_e';
    AddRatVector('xcA_e', '0.1 * ones(num_e, 1)', '', 'Vector of activation variables for e-neurons', relPred, 'length(xcA_e) == num_e');
    AddRatVector('xcB_e', '0.1 * ones(num_e, 1)', '', 'Vector of inactivation variables for e-neurons', relPred, 'length(xcB_e) == num_e');
    
    relPred = 'enableExtraCurrent_i';
    AddRatVector('xcA_i', '0.1 * ones(num_i, 1)', '', 'Vector of activation variables for i-neurons', relPred, 'length(xcA_i) == num_i');
    AddRatVector('xcB_i', '0.1 * ones(num_i, 1)', '', 'Vector of inactivation variables for i-neurons', relPred, 'length(xcB_i) == num_i');
    
end

