
function params = ParseStateBlock(scope)

    params = {};

    for i = 1 : length(scope)
        tmpStr = strsplit(scope{i}, ':');
        tmpStr = strtrim(tmpStr{1});
        tmpStr = regexprep(tmpStr, '(STATE\s*{)|(\(\S*\))|(<\S*>)|}', '');
        
        if isempty(tmpStr)
            continue;
        end
        
        tmpStr = strsplit(tmpStr, ' ');
        
        for j = 1 : length(tmpStr)     
            if ~isempty(tmpStr{j})
                 params{end + 1, 1} = strtrim(tmpStr{j}); %#ok<AGROW>
            end
        end
    end
end

