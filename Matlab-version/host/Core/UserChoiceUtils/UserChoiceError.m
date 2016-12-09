function UserChoiceError(fieldName)
%% Report the user choice error

    global mobileMode userChoices
    
    assert(mobileMode);
    
    if isempty(fieldnames(userChoices))
        error(...
            ['Usage of Matlab mobile requires one input argument to be passed in Arachne.\n', ...
            'The argument must be a struct with the next required field: ''%s''.'], fieldName);
    else
        error('The following field is missing in the input struct passed to Arachne: ''%s''', fieldName);
    end
    
end