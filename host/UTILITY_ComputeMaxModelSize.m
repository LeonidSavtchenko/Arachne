function UTILITY_ComputeMaxModelSize(varargin)
%% Compute the maximum model size (i.e. num_e and num_i) given physical memory limit and ratio num_e / num_i

    AddPaths();

    global mobileMode
    mobileMode = false;
    
    ComputeMaxModelSize(varargin{:});
    
end