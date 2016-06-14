function AddHpcParamsForTakingSnapshot()

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
    
    desc = {'How often Matlab host should check if HPC kernel has executed requested command?', ...
            '(There are two commands: terminate, dump snapshot.)', ...
            'The check will be done once per <b><i><font color="blue">c4ePeriodSec</font></i></b> seconds.'};
    AddPosRatScalar('c4ePeriodSec', 5, 's', desc);
    
    AddStringScalar('outFileName', '''output''', 'Output MAT-file name', true, 'valPred_outFileName(outFileName)', '.mat'); 
    
    AddBoolScalar('saveInput2Output', true, 'Whether to save input parameters to output MAT-file');
    
end