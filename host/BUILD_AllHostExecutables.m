function BUILD_AllHostExecutables()
%% Compile 8 executable files for all the entry points of the host.
%  Copy all dependent non-M files to the target directory. 

    AddPaths();
    
    % Create a new or clean up the existing target directory
    outDirPath = fullfile(cd, '..', 'host_compiled');
    PrepareEmptyDir(outDirPath);
    
    % Copy all dependent non-M files to the target directory
    
    disp('Copying non-M files ...');
    
    srcDir = fullfile(cd, 'Core');
    dstDir = fullfile(cd, '..', 'host_compiled', 'Core');
    
    if ispc
        CopyOneDir(dstDir, srcDir, '3rdparty');
        CopyOneDir(dstDir, srcDir, 'scripts', 'win-lin');
        CopyOneDir(dstDir, srcDir, 'scripts', 'win-win');
    elseif isunix
        CopyOneDir(dstDir, srcDir, 'scripts', 'lin-lin');
    else
        error('Unknown OS');
    end
    
    CopyOneDir(dstDir, srcDir, 'ModFileUtils', 'CppCodeTemplates');
    
    % Compile all the scripts

    scriptNames = {
        'START_GammaSimulator', ...
        'SCRIPT_CleanUp', ...
        'SCRIPT_KillBackgroundProcess', ...
        'SCRIPT_RecoverBackupProgress', ...
        'SCRIPT_TakeSnapshot', ...
        'SCRIPT_TerminateBackgroundProcess', ...
        'UTILITY_ComputeMaxModelSize', ...
        'UTILITY_PlotStdpModels'};
    
    for i = 1 : length(scriptNames)
        CompileOneExecutable(scriptNames{i});
    end
    
end

function CopyOneDir(dstDir, srcDir, varargin)

    copyfile(...
        fullfile(srcDir, varargin{:}), ...
        fullfile(dstDir, varargin{:}), ...
        'f');

end

function CompileOneExecutable(scriptName)
    
    disp(['Compiling executable for ', scriptName, ' ...']);
    
    % Output name for executable file
    outName = ['-o', ' ', scriptName];

    % Wrapper functions. For console application we need to use "main"
    wrapperFunc = ['-W main:', scriptName];

    % Link object files into a stand-alone executable
    linkParam = '-T link:exe';

    % Output directory. All generated files will be put in this directory
    outDirectory = ['-d', ' ', fullfile(cd, '..', 'host_compiled')];

    % Show compilation steps
    compInfo = '-v';
    
    % Do not embed the deployable archive in binaries
    deployBin = '-C';
    
    % Path to main M-file
    pathToMainFile = fullfile(cd, [scriptName, '.m']);

    % Attach directory
    attachDirectory = cd;
    idx = strfind(attachDirectory, filesep);
    attachDirectory = attachDirectory(1 : idx(end) - 1);
    attachDirectory = ['-a', ' ', attachDirectory];

    command = [...
        'mcc', ' ', ...
        outName, ' ', wrapperFunc, ' ', ...
        linkParam, ' ', outDirectory, ' ', ...
        compInfo, ' ', deployBin, ' ', ...
        pathToMainFile, ' ', attachDirectory];

    % !!
    % command = ['mcc', ' ', scriptName, '.m -m'];
    
    eval(command);
    
end