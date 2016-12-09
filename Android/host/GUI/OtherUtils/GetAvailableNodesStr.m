function str = GetAvailableNodesStr()
%% Get the available nodes cell array as a string that can be evaluated

    global availableNodes
    
    str = ['{''', strjoin(availableNodes, ''', '''), '''}'];
    
end