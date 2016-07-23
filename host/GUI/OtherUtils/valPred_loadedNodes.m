function valid = valPred_loadedNodes(loadedNodes)
%% Validation predicate for "loadedNodes" cell array of strings

    global idleMaster availableNodes
    global scalTest np maxNP
    
    if ~scalTest
        reqNumNodes = np;
    else
        reqNumNodes = maxNP;
    end
    valid = (length(loadedNodes) >= reqNumNodes);
    if ~valid
        % Not enough nodes
        return
    end
    
    idleMaster = true;
    slaveNodesCount = length(availableNodes);
    slaveNodes = false(1, slaveNodesCount);
    
    for node = loadedNodes
        matchFound = false;
        for i = 1 : slaveNodesCount
            if strcmp(node, availableNodes(i))
                if slaveNodes(i)
                    % duplicate slave node
                    valid = false;
                    break
                else
                    slaveNodes(i) = true;
                    matchFound = true;
                    break
                end
            end
        end
        if ~matchFound
            valid = false;
            break
        end
    end
    
end