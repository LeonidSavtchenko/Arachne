function valid = evalTextArea(text, varName)
%% Evaluate custom m-code from the editable text area.
%  Before the evaluation, get the global variable with the given name
%  so that it can be assigned during evaluation.

    eval(['global ', varName]);
    eval([varName, ' = struct;']);
    
    % Convert the text from cell array to char array,
    % because "eval" function does not support cell arrays
    [row, ~] = size(text);
    cmd = '';
    for i = 1 : row
        line = text(i, :);
        if iscell(line)
            line = line{1};
        end
        cmd = sprintf('%s\n%s', cmd, line);
    end
    
    try
        eval(cmd);
        valid = true;
    catch
        valid = false;
    end

end