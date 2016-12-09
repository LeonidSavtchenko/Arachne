function status = RecoverMatFiles(idx)
%% Recover files "output.mat" and "intermediate.mat" (if present) from the backup storage
%  specified by input argument (1 or 2)
    
    command = ScriptCallCommand('recover');
    
    command = [command, ' ', num2str(idx)];
    status = system(command);
    
end