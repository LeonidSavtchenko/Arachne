function valid = HasField(struct, index)
%% Check if given struct has a field pointed by the one-based index

    valid = false;

    if index < 1
        return
    end
    
    allNames = fieldnames(struct);
    
    if index > length(allNames)
        return
    end
    
    valid = true;
    
end