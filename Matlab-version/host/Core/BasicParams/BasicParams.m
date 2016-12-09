function BasicParams(varargin)
%% Initialize the basic parmeters, i.e. the parameters which must be set before showing GUI

    global mobileMode
    global remoteHPC availableNodes ramPerNode useGUI userChoices
    global debugMode
    
    if isempty(mobileMode)
        % temporary solution !!
        mobileMode = false;
    end
    
    % Initialize global structures
    InitializeStructs();
    
    % Validate input arguments
    ValidateInputArgs(varargin{:});
       
    % Whether to conduct simulation on remote HPC cluster
    remoteHPC = true;
    
    % Names of all cluster nodes you may want to use in simulation
    demoMode = true;
    if ~demoMode
        availableNodes = {'tuxm1', ...
                          'tuxm2', ...
                          'tuxm3', ...
                          'tuxm4', ...
                          'tuxm5', ...
                          'tuxm6', ...
                          'tuxm7', ...
                          'tuxm8', ...
                          'tuxm9', ...
                          'tuxm10', ...
                          'tuxm11', ...
                          'tuxm12'};
    else
        availableNodes = {'tuxmaster'};
    end
    
    % Minimum amount of physical memory installed on a cluster node (in megabytes).
    % The simulation will not start if more than 75% is expected to be used.
    ramPerNode = 8192;
    
    % Whether to use Graphic User Interface to prepare input parameters.
    % (If not, then default values will be used for all the parameters that are normally exposed with GUI.)
    useGUI = ~mobileMode;
    
    % The array of predefined user choices used instead of "input", "questdlg" and "uigetfile" functions calls
    if ~isempty(varargin)
        userChoices = varargin{1};
    else
        userChoices = struct();
    end
    
    debugMode = false;
    
end

function InitializeStructs()
%% Initialize global structures

    global GuiTypes ScmTypes SclModels
    global QuestionIds DialogIds FileSelectorIds
    
    GuiTypes = struct('StartFromScratch', 1, ...
                      'ContinueOldSession', 2, ...
                      'MonitorBackgroundProcess', 3, ...
                      'TakeOutputData', 4, ...
                      'ScriptTakeSnapshot', 5, ...
                      'UtilityPlotStdpModels', 6, ...
                      'UtilityComputeMaxModelSize', 7);
    
    ScmTypes = struct('AllZeros', 1, 'AllEqual', 2, 'HstDense', 3, 'HstSparse', 4, 'KrnDense', 5, 'KrnSparse', 6, 'KrnOneBit', 7, 'KrnInPlace', 8);
    
    SclModels = struct('BSS', 1, 'BSD', 2);
    
    % User choice structs
    QuestionIds = struct('WhatToDo', 1, 'AdditionalPeriod', 2);
    DialogIds = struct('ContinueWithBadParams', 1, 'ImageSource', 2, 'MemorizeOrRecall', 3);
    FileSelectorIds = struct('ModFiles', 1, 'ImageFile', 2);
    
end

function ValidateInputArgs(varargin)
%% Validate input arguments

    global QuestionIds DialogIds FileSelectorIds

    n = length(varargin);
    
    if n > 1
        error('Arachne is called with %i input arguments, but 0 or 1 is expected.', n);
    elseif n == 0
        return
    end
    
    arg = varargin{1};
    if ~isstruct(arg)
        error('Arachne is called with an input argument not being a struct.');
    end
    
    fieldNames = fieldnames(arg);

    % Prepare the error message pattern in advance
    delimiter = '''\n    ''';
    errMsgPat = [...
        'Arachne is called with the input struct that has the following unsupported field: ''%s''.\n', ...
        'Only the next fields are supported:\n    ''', ...
        strjoin(fieldnames(QuestionIds)', delimiter), delimiter, ...
        strjoin(fieldnames(DialogIds)', delimiter), delimiter, ...
        strjoin(fieldnames(FileSelectorIds)', delimiter), ''''];
    
    for i = 1 : length(fieldNames)
        
        fieldName = fieldNames{i};
        
        if ~isfield(QuestionIds, fieldName) && ...
           ~isfield(DialogIds, fieldName) && ...
           ~isfield(FileSelectorIds, fieldName)
            error(errMsgPat, fieldName);
        end
        
    end
    
end
        