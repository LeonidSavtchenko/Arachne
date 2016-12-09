function scenario = SelectScenario()

    global QuestionIds mobileMode
    
    % Input:
    global remoteHPC scalTest
    
    % Output:
    global Scenarios setIter continuationMode m_steps
    
    Scenarios = struct('PrepareParamsRunAndMonitor', 1, 'MonitorBackgroundProcess', 2, 'TakeAndVisualizeResults', 3, 'DoNothing', 4);
    
    setIter = true;

    % Check if HPC kernel is running in background mode
    disp('Checking whether HPC kernel is running in background mode ...');
    isRunning = false; % !!CheckIfRunning();
    if isRunning
        fprintf('It is.\n\n');
    else
        disp('It is not.');
    end
    
    if scalTest
        if isRunning
            msg = [...
                'Cannot do scalability test because another simulation process is running at the moment.\n', ...
                'Please wait until it completes or use script SCRIPT_TerminateBackgroundProcess.m or SCRIPT_KillBackgroundProcess.m to stop the simulation and then try to run the test once again.\n'];
            error(sprintf(msg)); %#ok<SPERR>
        end
        
        % Do scalability test
        continuationMode = false;
        scenario = Scenarios.PrepareParamsRunAndMonitor;
        return
    end

    if isRunning
        % Do indirect monitoring
        scenario = Scenarios.MonitorBackgroundProcess;
        return
    end
    
    % Check whether file "output.mat" exists
    disp('Checking whether file "output.mat" is present in HPC kernel output directory ...');
    isOutputPresent = false; % !!CheckFileExists('iofiles', 'kernel-host', 'output.mat');
    if isOutputPresent
        disp('It is.');

        % Check whether file "intermediate.mat" exists
        disp('Checking whether file "intermediate.mat" is present in HPC kernel output directory ...');
        isIntermPresent = false; % !!CheckFileExists('iofiles', 'kernel-kernel', 'intermediate.mat');

        if remoteHPC
            verb = 'Download';
        else
            verb = 'Take';
        end

        % Prepare invitation prompt for user input
        if isIntermPresent
            disp('It is.');
            prompt = [...
                '1. ', verb , ' "output.mat" and analyse results.\n', ...
                '2. Start simulation from scratch (current files "output.mat" and "intermediate.mat" will be lost).\n', ...
                '3. Continue simulation from the same point.\n', ...
                '[1, 2 or 3]: '];
        else
            disp('It is not.');
            prompt = [...
                '1. ', verb , ' "output.mat" and analyse results.\n', ...
                '2. Start simulation from scratch (current file "output.mat" will be lost).\n', ...
                '[1 or 2]: '];
        end
        prompt = ['\nPlease specify what to do:\n', prompt];

        % Loop until user inputs correct data
        questionId = QuestionIds.WhatToDo;
        while true
            reply = Input(questionId, prompt);
            if strcmp(reply, '1') || strcmp(reply, '2') || (isIntermPresent && strcmp(reply, '3'))
                fprintf('\n');
                break
            end
            if mobileMode
                % Prevent infinite loop
                fieldName = GetFieldName(QuestionIds, questionId);
                error('The input struct passed to Arachne has the following incorrect field: ''%s''.', fieldName);
            end
        end

        if strcmp(reply, '3')
            % HPC kernel will continue previous simulation session
            % ("input.mat", "intermediate.mat", and "output.mat" will be read)
            continuationMode = true;
        else
            % HPC kernel will start simulation from scratch
            % (only "input.mat" will be read)
            continuationMode = false;
        end

        if continuationMode
            prompt = 'Please specify the additional period to simulate (e.g. "1234.5 ms" or "12345 it"): ';
            % Loop until user inputs correct data
            questionId = QuestionIds.AdditionalPeriod;
            while true
                reply2 = Input(questionId, prompt);
                if length(reply2) < 4
                    continue
                end
                suffix = reply2(end - 2 : end);
                if strcmp(suffix, ' ms')
                    setIter = false;
                elseif strcmp(suffix, ' it')
                    setIter = true;
                else
                    continue
                end
                reply2(end - 2 : end) = [];
                m_steps = str2double(reply2);
                % Remark: str2double produces NaN if the string cannot be converted to a number
                if ~isnan(m_steps) && m_steps > 0 && (~setIter || rem(m_steps, 1) == 0)
                    fprintf('\n');
                    break
                end
                if mobileMode
                    % Prevent infinite loop
                    fieldName = GetFieldName(QuestionIds, questionId);
                    error('The input struct passed to Arachne has the following incorrect field: ''%s''.', fieldName);
                end
            end
        end
    else
        % The file "kernel-host/output.mat" is absent

        fprintf('It is not.\n');

        prompt = [...
            '\nPlease specify what to do:\n', ...
            '1. Start simulation from scratch (current backup files will be lost).\n', ...
            '2. Do nothing.\n', ...
            '[1 or 2]: '];

        % Loop until user inputs correct data
        questionId = QuestionIds.WhatToDo;
        while true
            reply = Input(questionId, prompt);
            if strcmp(reply, '1')
                break
            elseif strcmp(reply, '2')
                scenario = Scenarios.DoNothing;
                return
            end
            if mobileMode
                % Prevent infinite loop
                fieldName = GetFieldName(QuestionIds, questionId);
                error('The input struct passed to Arachne has the following incorrect field: ''%s''.', fieldName);
            end
        end

        fprintf('\n');

        reply = '2';
        continuationMode = false;
    end

    if ~strcmp(reply, '1')
        % Run HPC kernel in background or foreground mode
        % and do direct or indirect monitoring depending on remoteHPC
        scenario = Scenarios.PrepareParamsRunAndMonitor;
        return
    end
    
    scenario = Scenarios.TakeAndVisualizeResults;
    
end
