function HpcParamsRemainder()

    global remoteHPC backgroundMode scalTest
    global useSPA distMatPVH fakeMPI saveIntermMat class outFileName
    global hostParams idleMaster memPerNodeLimit
    global loadedNodes

    if useSPA
        class = 'single';
    else
        class = 'double'; %#ok<*UNRCH>
    end
    
    if remoteHPC
        % List of nodes that will be passed to the script
        hostParams = strjoin(loadedNodes, ',');

        % Limit of physical memory usage per cluster node (in megabytes).
        % The simulation will not start if the limit is exceeded.
        if idleMaster
            % The limit is implied by slave node RAM
            ram = 8192; 
        else
            % The limit is implied by master node RAM
            ram = 3072;
        end
        memPerNodeLimit = ram * 0.75;
    end

    %% Set values to some non-relevant, but used parameters
    
    if fakeMPI
        distMatPVH = false;
    end

    if distMatPVH
        saveIntermMat = false;
    end
    
    % Gamma simulator can be run in foreground remotely, but we disallow this mode by two reasons:
    % 1) The process is not killed automatically when connection is dropped between host and kernel.
    %    As a result, several foreground simulations can be launched in parallel causing file collisions in I/O directories.
    % 2) Host cannot monitor foreground simulation correctly after dropping of connection --
    %    it's considered as background simulation that does not respond.
    if scalTest
        backgroundMode = false;
    elseif remoteHPC
        backgroundMode = true;
    end
    
end