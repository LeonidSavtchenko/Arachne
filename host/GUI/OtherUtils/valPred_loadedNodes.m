function valid = valPred_loadedNodes(loadedNodes)
%% Validation predicate for "loadedNodes" cell array of strings

    global availableNodes
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
    
    nodesCount = length(availableNodes);
    nodesUsage = false(1, nodesCount);
    
    for node = loadedNodes
        matchFound = false;
        for i = 1 : nodesCount
            if strcmp(node, availableNodes(i))
                if nodesUsage(i)
                    % Duplicate node
                    valid = false;
                else
                    % Free node
                    nodesUsage(i) = true;
                    matchFound = true;
                end
                break
            end
        end
        if ~matchFound
            % Not found node
            valid = false;
            break
        end
    end
    
end