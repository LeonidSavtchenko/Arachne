function SCRIPT_MOBILE_TakeSnapshot(varargin)
%% Request HPC kernel running in background mode for dumping a snapshot
%  to the file "iofiles/kernel-host/snapshot/output.mat,"
%  grab the file and show the results to user.

    AddPaths();

    % Any GUI, "input", "questdlg" or "uigetfile" functions calls will be avoided,
    % because Matlab mobile does not support interaction with user at runtime.
    global mobileMode
    mobileMode = true;
    
    TakeSnapshot(varargin{:});
    
end
