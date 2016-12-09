function reply = Input(questionId, prompt)
%% Get user's answer to the given question.
%  This is basically a wrapper of "input" function that supports a predefined answer.
%  If we are called from Matlab mobile, then the predefined answer is required,
%  because Matlab mobile does not support interaction with user at runtime.

    global QuestionIds
    global userChoices mobileMode
    
    fieldName = GetFieldName(QuestionIds, questionId);
    try
        % Try to get a predefined answer
        reply = userChoices.(fieldName);
    catch
        if mobileMode
            % Interaction with user is impossible
            UserChoiceError(fieldName);
        else
            % The interaction
            reply = input(prompt, 's');
            return
        end
    end
    
    % Just for convenience so that user can specify a number for the predefined answer
    if isfloat(reply)
        reply = num2str(reply);
    end
    
end
