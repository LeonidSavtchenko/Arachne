function name = GetFieldName(struct, index)
%% Gets the name of the struct field given one-based index

    allNames = fieldnames(struct);
    name = allNames{index};
    
end