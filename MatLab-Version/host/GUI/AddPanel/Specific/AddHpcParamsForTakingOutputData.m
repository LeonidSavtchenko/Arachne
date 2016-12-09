function AddHpcParamsForTakingOutputData()

    global remoteHPC

    AddPanel('HPC');
    
    desc = {'Whether to use remote High Performance Computing server', ...
            '<b>unchecked:</b> <i>call HPC kernel on this machine/cluster</i>', ...
            '<b>checked:</b> <i>call HPC kernel on remote machine/cluster</i>'};
    AddBoolScalar('remoteHPC', remoteHPC, desc);
    
    desc = {'Whether input/output MAT-files should be zipped before and unzipped after transferring between local machine and head node of the cluster.', ...
            '<b>unchecked:</b> <i>the files are transferred without compressing</i>', ...
            '<b>checked:</b> <i>the files are compressed before and decompressed after transferring</i>'};
    AddBoolScalar('zipMatFiles', true, desc, 'remoteHPC');
    
    AddStringScalar('outFileName', '''output''', 'Output MAT-file name', true, 'valPred_outFileName(outFileName)', '.mat'); 
    
    AddBoolScalar('saveInput2Output', true, 'Whether to save input parameters to output MAT-file');
    
end