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
    slaveNodesCount = length(availableNodes) - 1;
    slaveNodes = false(1, slaveNodesCount);
    
    for node = loadedNodes
        % check whether it is master or slave node
        if strcmp(node, 'tuxmaster')
            if ~idleMaster
                % duplicate master node
                valid = false;
                break
            else
                idleMaster = false;
            end
        else
            matchFound = false;
            for i = 0 : slaveNodesCount
                if strcmp(node, strcat('tuxm', num2str(i)))                   
                    if slaveNodes(1, i)
                        % duplicate slave node
                        valid = false;
                        break
                    else
                        slaveNodes(1, i) = true;
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
    
end