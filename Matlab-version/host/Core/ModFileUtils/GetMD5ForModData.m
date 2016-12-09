function hashStr = GetMD5ForModData()
%% Compute MD5-code based on content of the selected MOD-files and values of the assigned parameters

    global importMod_e modParams_e modFileNames_e	%#ok<NUSED>
    global importMod_i modParams_i modFileNames_i	%#ok<NUSED>
    global modDirPath_e
    global modDirPath_i

    % Prepare a temporary MAT-file
    tempMatFilePathName = fullfile(cd, 'md5Temp.mat');
    save(tempMatFilePathName, ...
        'importMod_e', 'modFileNames_e', 'modParams_e', ...
        'importMod_i', 'modFileNames_e', 'modParams_i');
    
    n1 = length(modFileNames_e);
    n2 = length(modFileNames_i);
    filePathNames = cell(1 + n1 + n2, 1);
    
    filePathNames{1} = tempMatFilePathName;
    
    ext = '.mod';
    
    % Prepare paths of MOD-files for e-neurons
    for i = 1 : n1
        filePathNames{1 + i} = fullfile(modDirPath_e, [modFileNames_e{i}, ext]);
    end
    
    % Prepare paths of MOD-files for i-neurons
    i0 = 1 + n1;
    for i = 1 : n2
        filePathNames{i0 + i} = fullfile(modDirPath_i, [modFileNames_i{i}, ext]);
    end
    
    hashStr = GetMD5ForFiles(filePathNames);
    
    delete(tempMatFilePathName);

end

function hashStr = GetMD5ForFiles(filePathNames)
%% Compute MD5-code based on content of the files
%  http://stackoverflow.com/questions/12140458/calculating-md5-hash-rfc-1321-conform-in-matlab-via-java
    
    mdDigest = java.security.MessageDigest.getInstance('MD5');
    
    hdrSize = 128;
    bufSize = 8192;

    for i = 1 : length(filePathNames)
        filePathName = filePathNames{i};
        
        fid = fopen(filePathName);
        assert(fid ~= -1);
        
        % Ignore the title of mat file, for example:
        % MATLAB 5.0 MAT-file, Platform: PCWIN64, Created on: Fri Dec 02 19:42:50 2016
        fread(fid, hdrSize, '*uint8');

        while ~feof(fid)
            [currData, len] = fread(fid, bufSize, '*uint8');
            if ~isempty(currData)
                mdDigest.update(currData, 0, len);
            end
        end

        fclose(fid);
    end
    
    hashInt8 = mdDigest.digest();
    hashUInt8 = typecast(hashInt8, 'uint8');
    hashHexStr2d = dec2hex(hashUInt8);
    hashHexStr1d = reshape(hashHexStr2d, 1, []);
    hashStr = lower(hashHexStr1d);
    
end