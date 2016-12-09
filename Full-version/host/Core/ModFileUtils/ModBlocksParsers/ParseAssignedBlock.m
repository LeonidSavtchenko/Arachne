function params = ParseAssignedBlock(scope)

    params = {};

    for i = 1 : length(scope)      
        tmpStr = scope{i};
        if ~(~isempty(tmpStr) && isempty(strfind(tmpStr, '{')) && isempty(strfind(tmpStr, '}')))
            continue
        end
        
        tmpStr = strsplit(tmpStr, ':');
        tmpStr = tmpStr{1};
        splitTmpStr = strsplit(regexprep(tmpStr, '\(\S*\)', ''));

        for j = 1 : length(splitTmpStr)           
            cStr = splitTmpStr{j};
            if ~(isempty(cStr) || strcmp(cStr, 'v'))
                 params{end + 1, 1} = cStr; %#ok<AGROW>
            end
        end
    end
end
