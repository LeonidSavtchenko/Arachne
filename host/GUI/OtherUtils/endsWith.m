function result = endsWith(str, suffix)
%% Check if the string ends with the specified suffix

    n = length(suffix);
    if length(str) < n
        result = false;
    else
        result = strcmp(str(end-n+1 : end), suffix);
    end

end