function [fileName, pathName, filterIndex] = UIGetFile(fileSelectorId, varargin)
%% Let user select file(s).
%  This is basically a wrapper of "uigetfile" function that supports a predefined choice.
%  If we are called from Matlab mobile, then the predefined choice is required,
%  because Matlab mobile does not support interaction with user at runtime.

    global FileSelectorIds
    global userChoices mobileMode
        
    fieldName = GetFieldName(FileSelectorIds, fileSelectorId);
    try
        % Try to get a predefined choice
    	filePath = userChoices.(fieldName);
    catch
        if mobileMode
            % Interaction with user is impossible
            UserChoiceError(fieldName);
        else
            % The interaction
            [fileName, pathName, filterIndex] = uigetfile(varargin{:});
            return
        end
    end

    [pathName, fileName, fileExt] = fileparts(filePath);
    fileName = [fileName, fileExt];
    filterIndex = 1;

end