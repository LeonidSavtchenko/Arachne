function GenerateCumulativeCppSrcFile(modFileNames_e, modFileNames_i, outDirPath)

    outFileName = 'AllModCurrents.cpp';
    
    fprintf('Generating %s ...\n', outFileName);
    
    numLines = 13;
    outFileLines = {numLines, 1};
    
    outFileLines{1, 1} = '#include "AllModCurrents.h"'; 
    outFileLines{2, 1} = '';
    
    lenModFileNames_e = length(modFileNames_e);
    lenModFileNames_i = length(modFileNames_i);
    
    if lenModFileNames_e > 0
        outFileLines{3, 1} = 'template';
        outFileLines{4, 1} = 'class AllModCurrents_e<float>;';
        outFileLines{5, 1} = '';
        outFileLines{6, 1} = 'template';
        outFileLines{7, 1} = 'class AllModCurrents_e<double>;';
    else
        outFileLines{3, 1} = '';
        outFileLines{4, 1} = '';
        outFileLines{5, 1} = '';
        outFileLines{6, 1} = '';
        outFileLines{7, 1} = '';
    end
    
    outFileLines{8, 1} = '';
    
    if lenModFileNames_i > 0
        outFileLines{9, 1} = 'template';
        outFileLines{10, 1} = 'class AllModCurrents_i<float>;';
        outFileLines{11, 1} = '';
        outFileLines{12, 1} = 'template';
        outFileLines{13, 1} = 'class AllModCurrents_i<double>;';
    else
        outFileLines{9, 1} = '';
        outFileLines{10, 1} = '';
        outFileLines{11, 1} = '';
        outFileLines{12, 1} = '';
        outFileLines{13, 1} = '';
    end

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
