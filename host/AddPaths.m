function AddPaths()

    addpath(genpath(cd));
    rmpath([cd, '\Code\3rdparty']);
    rmpath(genpath([cd, '\Code\scripts']));
    
end