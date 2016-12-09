function idx = SelectBackupStorage()
%% Decide whether backup storage 1 or backup storage 2 should be used to recover from

    % Check whether backup storage pointer file "1" exists
    isPresent1 = CheckFileExists('iofiles', 'backup', '1');
    
    % Check whether backup storage pointer file "2" exists
    isPresent2 = CheckFileExists('iofiles', 'backup', '2');
    
    if isPresent1 && ~isPresent2
        % Backup storage 2 will be used for recovery.
        % (Existence of the file with name "1" indicates that
        % backup storage 1 should be used for the next backup saving.)
        idx = 2;
        return
    elseif isPresent2 && ~isPresent1
        % Backup storage 1 will be used for recovery.
        % (Existence of the file with name "2" indicates that
        % backup storage 2 should be used for the next backup saving.)
        idx = 1;
        return
    end
    
    % Backup storage pointer file is spoiled.
    % That means that crash of simulation happened at the moment of saving of the file.
    % Check if backup storages contain backup files.
    
    disp('Backup storage pointer file is spoiled.');
    
    isPresent1 = CheckFileExists('iofiles', 'backup', 'backup-1', 'kernel-host', 'output.mat');
    isPresent2 = CheckFileExists('iofiles', 'backup', 'backup-2', 'kernel-host', 'output.mat');
    if ~isPresent1 && ~isPresent2
        error('No one backup storage contains a file "output.mat" to recover.');
    elseif ~isPresent2
        % Backup storage 1 will be used for recovery
        idx = 1;
        return
    elseif ~isPresent1
        % Backup storage 2 will be used for recovery
        idx = 2;
        return
    end
    
    % Both backup storages contain files "output.mat".
    % In this case we need to determine what storage contains bigger backup file
    % and use it for recovery (the bigger, the latter).
    
    command = ScriptCallCommand('get_size');
    
    % Get size of the file "output.mat" in backup storage 1
    filePath = 'backup-1/kernel-host/output.mat';
    [status, size1] = system([command, ' ', filePath]);
    if status ~= 0
        error('Failed to get size of the file "iofiles/backup/backup-1/kernel-host/output.mat" (1).');
    end
    size1 = str2double(size1);
    if isnan(size1) || size1 <= 0
        error('Failed to get size of the file "iofiles/backup/backup-1/kernel-host/output.mat" (2).');
    end
    
    % Get size of the file "output.mat" in backup storage 2
    filePath = 'backup-2/kernel-host/output.mat';
    [status, size2] = system([command, ' ', filePath]);
    if status ~= 0
        error('Failed to get size of the file "iofiles/backup/backup-2/kernel-host/output.mat" (1).');
    end
    size2 = str2double(size2);
    if isnan(size2) || size2 <= 0
        error('Failed to get size of the file "iofiles/backup/backup-2/kernel-host/output.mat" (2).');
    end
    
    if size1 > size2
        % Backup storage 1 will be used for recovery
        idx = 1;
    elseif size2 > size1
        % Backup storage 2 will be used
        idx = 2;
    else
        msg = sprintf([...
            'Both backup files "output.mat" have the same size.\n', ...
            'Cannot decide whether backup storage 1 or 2 should be used for recovery.']);
        error(msg); %#ok<SPERR>
        % Justification: error does not recognize '\n'
    end
    
end