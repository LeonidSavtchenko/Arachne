function AddHpcParams()

    global ScmTypes scalTest remoteHPC
    
    AddPanel('HPC');
    
    AddBoolScalar('fakeMPI', false, 'Whether to use the fake MPI version of gamma sumulator');
    
    desc = {'On/off scalability test', ...
            '<b>unchecked:</b> <i>run HPC kernel once for number of processes equal <b><font color="blue">np</font></b> and number of threads equal <b><font color="blue">nt</font></b></i>', ...
            '<b>checked:</b> <i>run HPC kernel sequentially for number of processes equal <b><font color="blue">minNP</font></b>, <b><font color="blue">minNP</font></b> + 1, ..., <b><font color="blue">maxNP</font></b> and number of threads equal 1, 2, ..., <b><font color="blue">maxNT</font></b>'};
    AddBoolScalar('scalTest', scalTest, desc);
    AddPosIntScalar('np', 1, 'Number of MPI processes', '~scalTest && ~fakeMPI');
    AddPosIntScalar('nt', 4, 'Number of OMP threads per MPI process', '~scalTest');
    relPred = 'minNP < maxNP || (minNP == maxNP && maxNT > 1)';
    AddPosIntScalar('minNP', 1, 'Minimum number of MPI processes', 'scalTest', relPred);
    AddPosIntScalar('maxNP', 4, 'Maximum number of MPI processes', 'scalTest', relPred);
    AddPosIntScalar('maxNT', 4, 'Maximum number of OMP threads per MPI process', 'scalTest', relPred);
    
    desc = {'Whether to use remote High Performance Computing server', ...
            '<b>unchecked:</b> <i>call HPC kernel on this machine/cluster</i>', ...
            '<b>checked:</b> <i>call HPC kernel on remote machine/cluster</i>'};
    AddBoolScalar('remoteHPC', remoteHPC, desc, '~fakeMPI');
    AddStringVector('loadedNodes', '{''tuxm1'', ''tuxm2'', ''tuxm3'', ''tuxm4'', ''tuxm5'', ''tuxm6'', ''tuxm7'', ''tuxm8'', ''tuxm9'', ''tuxm10'', ''tuxm11'', ''tuxm12''}', 'List of nodes to run simulation on', 'remoteHPC && ~fakeMPI', 'valPred_loadedNodes(loadedNodes)');
    
    desc = {'Whether input/output MAT-files should be zipped before and unzipped after transferring between local machine and head node of the cluster.', ...
            '<b>unchecked:</b> <i>the files are transferred without compressing</i>', ...
            '<b>checked:</b> <i>the files are compressed before and decompressed after transferring</i>'};
    AddBoolScalar('zipMatFiles', true, desc, 'remoteHPC');
    
    scmTypesList = fieldnames(ScmTypes);
    scmTypesDesc = {'The matrix is not generated explicitly. <br>All elements are zeros', ...
                    'The matrix is not generated explicitly. <br>All elements are equal', ...
                    'The matrix is generated in Matlab and saved to input MAT-file with other data. <br>The matrix is generated in dense form', ...
                    'The matrix is generated in Matlab and saved to input MAT-file with other data. <br>The matrix is generated in sparse form', ...
                    'The matrix is generated in HPC kernel before 1st iteration already in distributed form. <br>An element of the matrix is stored in floating-point-number format', ...
                    'The matrix is generated in HPC kernel before 1st iteration already in distributed form. <br>The matrix is generated in sparse form', ...
                    'The matrix is generated in HPC kernel before 1st iteration already in distributed form. <br>An element of the matrix is stored in one bit', ...
                    'The matrix is re-generated in HPC kernel on each iteration'};
    AddList('scmType_ee', scmTypesList, ScmTypes.KrnDense, 'Type of synaptic conductance matrix', scmTypesDesc);
    AddList('scmType_ei', scmTypesList, ScmTypes.KrnDense, 'Type of synaptic conductance matrix', scmTypesDesc);
    AddList('scmType_ie', scmTypesList, ScmTypes.KrnDense, 'Type of synaptic conductance matrix', scmTypesDesc);
    AddList('scmType_ii', scmTypesList, ScmTypes.KrnDense, 'Type of synaptic conductance matrix', scmTypesDesc);
    
    AddBoolScalar('useSPA', true, 'Use Single Precision Arithmetics instead of double precision one');
    
    desc = {'In what mode matrices of presynaptic voltage history will be presented', ...
            '<b>unchecked:</b> <i>local mode</i>', ...
            '<b>checked:</b> <i>distributed mode</i>'};
    
    AddBoolScalar('distMatPVH', false, desc, '~(fakeMPI || remoteHPC)');
    
    desc = {'How often HPC kernel should report its progress?', ...
            'The report will be done once per <b><i><font color="blue">reportPeriodIter</font></i></b> iterations.'};
    AddIterCounter('reportPeriod', 100, desc);
    
    desc = {'Whether to save intermediate data file "intermediate.mat that makes it possible to stop simulation and continue afterwards from the same point.', ...
            'The file is saved when:', ...
            '1) specified <b><font color="blue">t_final</font></b> is reached;', ...
            '2) termination is requested by user;', ...
            '3) <b><font color="blue">saveBackupMats</font></b> == true and current iteration number is evenly divisible by <b><font color="blue">backupPeriodIter</font></b>.'};
    AddBoolScalar('saveIntermMat', true, desc, '~distMatPVH');
    
    desc = {'Whether backup files should be saved periodically.', ...
            'If <b><font color="blue">saveIntermMat</font></b> == false, then only "output.mat" is saved,', ...
            'if <b><font color="blue">saveIntermMat</font></b> == true, then both "output.mat" and "intermediate.mat" are saved periodically.'};
    AddBoolScalar('saveBackupMats', false, desc);
    
    desc = {'How often backup files should be saved?', ...
            'The saving will be done once per <b><i><font color="blue">backupPeriodIter</font></i></b> iterations.'};
    AddIterCounter('backupPeriod', 10000, desc, 'saveBackupMats');
    
    AddBoolScalar('backgroundMode', remoteHPC, 'Whether to run HPC kernel in background mode', '~(scalTest || remoteHPC)');
    
    desc = {'How often Matlab host should check the status of HPC kernel?', ...
            '(The status check means that the host program determines if HPC kernel is running and if so,', ...
            'what the current iteration number is.)', ...
            'The status will be checked once per <b><i><font color="blue">c4sPeriodSec</font></i></b> seconds.'};
    AddPosRatScalar('c4sPeriodSec', 5, 's', desc, 'backgroundMode');
    
    desc = {'How often HPC kernel should check for a command from Matlab host?', ...
            '(There are two commands: terminate, dump snapshot.)', ...
            'The check will be done once per <b><i><font color="blue">c4cPeriodIter</font></i></b> iterations.'};
    AddIterCounter('c4cPeriod', 100, desc);
    
    desc = {'How often Matlab host should check if HPC kernel has executed requested command?', ...
            '(There are two commands: terminate, dump snapshot.)', ...
            'The check will be done once per <b><i><font color="blue">c4ePeriodSec</font></i></b> seconds.'};
    AddPosRatScalar('c4ePeriodSec', 5, 's', desc);
    
    AddStringScalar('outFileName', '''output''', 'Output MAT-file name', true, 'valPred_outFileName(outFileName)', '.mat'); 
    
    AddBoolScalar('saveInput2Output', true, 'Whether to save input parameters to output MAT-file');
    
    desc = {'The limit for number of spikes per one simulation session.', ...
            'It will be used to preallocate the following arrays in HPC kernel: idx_e_spikes, t_e_spikes and other.', ...
            'The limit for number of spikes of a type is computed in HPC kernel as:', ...
            'max_num_spikes_e = int32(<b><font color="blue">num_e</font></b> * <b><font color="blue">m_steps</font></b> * <b><font color="blue">max_num_spikes_e_factor</font></b>);'};
    AddPosRatScalar('max_num_spikes_e_factor', 0.02, '', desc);
    
    desc = {'The limit for number of spikes per one simulation session.', ...
            'It will be used to preallocate the following arrays in HPC kernel: idx_i_spikes, t_i_spikes and other.', ...
            'The limit for number of spikes of a type is computed in HPC kernel as:', ...
            'max_num_spikes_i = int32(<b><font color="blue">num_i</font></b> * <b><font color="blue">m_steps</font></b> * <b><font color="blue">max_num_spikes_i_factor</font></b>);'};
    AddPosRatScalar('max_num_spikes_i_factor', 0.02, '', desc);
    
end