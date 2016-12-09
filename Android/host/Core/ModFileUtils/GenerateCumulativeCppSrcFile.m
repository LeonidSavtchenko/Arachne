function GenerateCumulativeCppSrcFile(modFileNames_e, modFileNames_i, outDirPath)

    outFileName = 'AllModCurrents.cpp';
    
    fprintf('    Generating %s ...\n', outFileName);
    
    lenModFileNames_e = length(modFileNames_e);
    lenModFileNames_i = length(modFileNames_i);
    
    outFileLines = {};
    
    outFileLines{end + 1, 1} = '#include "AllModCurrents.h"';
    outFileLines{end + 1, 1} = '';   
    for i = 1 : lenModFileNames_e
        outFileLines{end + 1, 1} = sprintf('#include "%s.cpp"', modFileNames_e{i});
    end
    outFileLines{end + 1, 1} = '';
    for i = 1 : lenModFileNames_i
        outFileLines{end + 1, 1} = sprintf('#include "%s.cpp"', modFileNames_i{i});
    end
    outFileLines{end + 1, 1} = '';
    outFileLines{end + 1, 1} = 'template';
    outFileLines{end + 1, 1} = 'class AllModCurrents_e<float>;';
    outFileLines{end + 1, 1} = '';
    outFileLines{end + 1, 1} = 'template';
    outFileLines{end + 1, 1} = 'class AllModCurrents_e<double>;';
    
    outFileLines{end + 1, 1} = '';
    
    outFileLines{end + 1, 1} = 'template';
    outFileLines{end + 1, 1} = 'class AllModCurrents_i<float>;';
    outFileLines{end + 1, 1} = '';
    outFileLines{end + 1, 1} = 'template';
    outFileLines{end + 1, 1} = 'class AllModCurrents_i<double>;';
    
    outFilePath = fullfile(outDirPath, outFileName);
    fid = fopen(outFilePath, 'w+');
    if fid == -1
        error('Cannot open file for writing');
    end
    
    for i = 1 : length(outFileLines)
        fprintf(fid, '%s\r\n', outFileLines{i});
    end
    
    fclose(fid);
     
end
