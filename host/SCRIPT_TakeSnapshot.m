function SCRIPT_TakeSnapshot()
%% Request HPC kernel running in background mode for dumping a snapshot
%  to the file "iofiles/kernel-host/snapshot/output.mat,"
%  grab the file and show the results to user.

    global guiType GuiTypes
    global pushbutton_OK_Delegate
    
    AddPaths();
    
    BasicParams();
    
    guiType = GuiTypes.ScriptTakeSnapshot;
    pushbutton_OK_Delegate = @SCRIPT_Core;
    
    PrepareParamsWithGUI();
    
end

function SCRIPT_Core()
 
    global c4ePeriodSec

    % Make sure that the kernel is running at the moment
    disp('Checking whether HPC kernel is running in background mode ...');
    isRunning = CheckIfRunning();
    if ~isRunning
        disp('HPC kernel process is not active.');
        disp('You can launch the script "START_GammaSimulator.m" to grab simulation results.');
        return
    end

    command = ScriptCallCommand('send_command');

    disp('Requesting HPC kernel process for dumping a snapshot ...');
    status = system([command, ' snapshot']);
    if status ~= 0
        error('Failed to do the request.');
    end
    disp('Request submitted.');

    while true
        % Wait
        pause(c4ePeriodSec);

        % Check if HPC kernel has dumped a snapshot
        isPresent = CheckFileExists('iofiles', 'kernel-host', 'snapshot', 'output.mat');
        if ~isPresent
            disp('HPC kernel has not dumped a snapshot yet ...');
        else
            disp('HPC kernel has dumped a snapshot.');
            break
        end
    end

    %% Grab output MAT-file from remote cluster or HPC kernel directory
    getFromSnapshot = true;
    GetOutputMat(getFromSnapshot);

    %% Read output MAT-file from disk and visualize current results of simulation
    ReadAndVisualizeResults();

    %% Clean up HPC kernel snapshot directory
    DeleteSnapshot();

end