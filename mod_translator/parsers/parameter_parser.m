function params = parameter_parser(scope)

    params = {};

    for i = 2 : length(scope) - 1
        pars = strsplit(scope{i}, {'(', ')', '<', '>'});
        param = regexprep(pars{1}, '\s', '');
        if ~isempty(param)
            params{end + 1, 1} = param; %#ok<AGROW>
        end
    end
end
