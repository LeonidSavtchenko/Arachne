function valid = evalTextArea(text)
%% Evaluate custom m-code from the editable text area.
%  The global struct "customVars" is initialized.
%  The left-hand argument "valid" equals false if there was an error during the code evaluation, and true otherwise.

    global customVars
    
    customVars = struct; % clear customVars before processing

    [row, ~] = size(text);
    cmd = '';
    for i = 1 : row
        cmd = sprintf('%s\n%s', cmd, text(i, :));
    end
    
    try
        eval(cmd);
        valid = true;
    catch
        valid = false;
    end

end