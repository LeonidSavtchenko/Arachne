function params = ParseAssignedBlock(scope)

    params = {};

    for i = 1 : length(scope)

        if ~(~isempty(scope{i}) && isempty(strfind(scope{i}, '{')) && isempty(strfind(scope{i}, '}')))
            continue
        end

        tmpStr = strsplit(regexprep(scope{i}, '\(\S*\)', ''));

        for j = 1 : length(tmpStr)

            if isempty(tmpStr{j})
                continue
                % variable 'v' (voltage) we process separately
            elseif ~strcmp(tmpStr{j}, 'v')
                params{end + 1, 1} = tmpStr{j};
            end
        end
    end
end
