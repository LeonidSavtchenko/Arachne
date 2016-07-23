function DeleteSnapshot()
%% Delete the snapshot dumped by HPC kernel to the directory "iofiles/kernel-host/snapshot"

    command = ScriptCallCommand('delete_snapshot');

    fprintf('\nCleaning up HPC kernel snapshot directory ...\n');
    
    status = system(command);
    if status ~= 0
        error('Failed to clean up HPC kernel snapshot directory.');
    end
    
end