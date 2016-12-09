function params = ParseParameterBlock(scope)

    params = {};

    for i = 1 : length(scope)    
        tmpStr = regexprep(scope{i}, '(\s*[A-Z]*\s*\{\s*)|\}', '');
        if isempty (tmpStr)
            continue
        end
        tmpStr = strsplit(tmpStr, ':');
        tmpStr = tmpStr{1};
        tmpStr = regexprep(tmpStr, '\(\S*\)', ';');
        pars = strsplit(regexprep(tmpStr, '<\S*>', ''), ';');
        
        for j = 1 : length(pars)
            
            tmpStr = regexprep(pars{j}, '\s+', '');
            
            if ~isempty(tmpStr)
                params{end + 1, 1} = tmpStr; %#ok<AGROW>
            end
        end
    end
end
