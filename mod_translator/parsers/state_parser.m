function params = state_parser(scope)

    params = {};
    tmp_str = '';

    for i = 1 : length(scope)
        tmp_str = [tmp_str, ' ', scope{i}];
    end

    left_idx = strfind(tmp_str, '{');
    left_idx = left_idx(1) + 1;

    right_idx = strfind(tmp_str, '}');
    right_idx = right_idx(end) - 1;

    tmp_str = tmp_str(left_idx : right_idx);

    tmp_params = strsplit(tmp_str);

    for i = 1 : length(tmp_params)

        if isempty(tmp_params{i})
            continue
        end

        params{end + 1, 1} = tmp_params{i};
    end
end
