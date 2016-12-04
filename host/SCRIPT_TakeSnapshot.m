function SCRIPT_TakeSnapshot(varargin)
%% Request HPC kernel running in background mode for dumping a snapshot
%  to the file "iofiles/kernel-host/snapshot/output.mat,"
%  grab the file and show the results to user.

     AddPaths();

    global mobileMode
    mobileMode = false;
    
    TakeSnapshot(varargin{:});
    
end 