function PrepareEmptyDir(path)
%% Given path, create a new or clean up the existing directory 

    [success, message, id] = rmdir(path, 's');      
    if ~success && ~strcmp(id, 'MATLAB:RMDIR:NotADirectory');
        error(message);
    end
    
	[success, message, ~] = mkdir(path);  
    if ~success
        error(message);
    end
    
end