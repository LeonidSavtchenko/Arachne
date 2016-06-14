function AddHpcParamsForMonitoring()

    global remoteHPC backgroundMode
    
    AddPanel('HPC');
    
    desc = {'Whether to use remote High Performance Computing server', ...
            '<b>unchecked:</b> <i>call HPC kernel on this machine/cluster</i>', ...
            '<b>checked:</b> <i>call HPC kernel on remote machine/cluster</i>'};
    AddBoolScalar('remoteHPC', remoteHPC, desc);
    
    desc = {'Whether input/output MAT-files should be zipped before and unzipped after transferring between local machine and head node of the cluster.', ...
            '<b>unchecked:</b> <i>the files are transferred without compressing</i>', ...
            '<b>checked:</b> <i>the files are compressed before and decompressed after transferring</i>'};
    AddBoolScalar('zipMatFiles', true, desc, 'remoteHPC');
    
    backgroundMode = true;
    
    desc = {'How often Matlab host should check the status of HPC kernel?', ...
            '(The status check means that the host program determines if HPC kernel is running and if so,', ...
            'what the current iteration number is.)', ...
            'The status will be checked once per <b><i><font color="blue">c4sPeriodSec</font></i></b> seconds.'};
    AddPosRatScalar('c4sPeriodSec', 5, 's', desc, 'backgroundMode');
    
    AddStringScalar('outFileName', '''output''', 'Output MAT-file name', true, 'valPred_outFileName(outFileName)', '.mat'); 
    
    AddBoolScalar('saveInput2Output', true, 'Whether to save input parameters to output MAT-file');
    
end