function AddModelAndHpcParamsForContinuation()

    global remoteHPC
    
    AddPanel('Model and HPC');
    
    AddBoolScalar('imageMode', true, 'Whether to apply image drive to e-cells');
    
    AddIntScalar('num_e', 200, 'Number of E-cells', 'imageMode', 'num_e > 1');
    
    AddBoolScalar('fakeMPI', false, 'Whether to use the fake MPI version of gamma sumulator');
    
    AddPosIntScalar('np', 1, 'Number of MPI processes');
    AddPosIntScalar('nt', 4, 'Number of OMP threads per MPI process');
    
    desc = {'Whether to use remote High Performance Computing server', ...
            '<b>unchecked:</b> <i>call HPC kernel on this machine/cluster</i>', ...
            '<b>checked:</b> <i>call HPC kernel on remote machine/cluster</i>'};
    AddBoolScalar('remoteHPC', remoteHPC, desc, '~fakeMPI');
    AddStringVector('loadedNodes', '{''tuxmaster'', ''tuxm1'', ''tuxm2'', ''tuxm3'', ''tuxm4'', ''tuxm5'', ''tuxm6'', ''tuxm7'', ''tuxm8'', ''tuxm9'', ''tuxm10'', ''tuxm11'', ''tuxm12''}', 'List of nodes to run simulation on', 'remoteHPC', 'valPred_loadedNodes(loadedNodes)');
    
    desc = {'Whether input/output MAT-files should be zipped before and unzipped after transferring between local machine and head node of the cluster.', ...
            '<b>unchecked:</b> <i>the files are transferred without compressing</i>', ...
            '<b>checked:</b> <i>the files are compressed before and decompressed after transferring</i>'};
    AddBoolScalar('zipMatFiles', true, desc, 'remoteHPC');
    
    AddBoolScalar('backgroundMode', remoteHPC, 'Whether to run HPC kernel in background mode');
    
    desc = {'How often Matlab host should check the status of HPC kernel?', ...
            '(The status check means that the host program determines if HPC kernel is running and if so,', ...
            'what the current iteration number is.)', ...
            'The status will be checked once per <b><i><font color="blue">c4sPeriodSec</font></i></b> seconds.'};
    AddPosRatScalar('c4sPeriodSec', 5, 's', desc, 'backgroundMode');
    
    AddStringScalar('outFileName', '''output''', 'Output MAT-file name', true, 'valPred_outFileName(outFileName)', '.mat'); 
    
    AddBoolScalar('saveInput2Output', true, 'Whether to save input parameters to output MAT-file');
    
end