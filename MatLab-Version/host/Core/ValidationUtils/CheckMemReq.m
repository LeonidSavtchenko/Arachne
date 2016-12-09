function CheckMemReq()
%% Check memory requirements taking into scope that
%  the matrices are the biggest objects used in simulation.
%  Do not start simulation if there is no enough physical memory on cluster nodes.
    
    global remoteHPC memPerNodeLimit
    
    if ~remoteHPC
        return
    end
    
    reqMemPerNode = CountTotalMemReq();
        
    msg = sprintf('Estimate for required physical memory per cluster node: %i MB', int32(reqMemPerNode));
    disp(msg);
    
    % Compare required memory with the limit specified in MB
    if reqMemPerNode > memPerNodeLimit
        error('There is no enough physical memory on a cluster node.');
    end
        
end