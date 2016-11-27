function buttonName = QuestDlg(dialogId, question,   title, btn1, btn2, btn3, default)
%% Get user's answer to the given question.
%  This is basically a wrapper of "questdlg" function that supports a predefined choice.
%  If we are called from Matlab mobile, then the predefined choice is required,
%  because Matlab mobile does not support interaction with user at runtime.

    global DialogIds
    global userChoices mobileMode
    
    if mobileMode && dialogId == DialogIds.ImageSource
        buttonName = 'Load';
        return
    end
    
    fieldName = GetFieldName(DialogIds, dialogId);
    try
        % Try to get a predefined choice
    	buttonName = userChoices.(fieldName);
    catch
        if mobileMode
            % Interaction with user is impossible
            UserChoiceError(fieldName);
        else
            % The interaction
            buttonName = questdlg(question, title, btn1, btn2, btn3, default);
        end
    end
    
end
