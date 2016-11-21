function GenerateCumulativeCppHdrFile(modFileNames_e, modFileNames_i, outDirPath)

    outFileName = 'AllModCurrents.h';
    
    fprintf('Generating %s ...\n', outFileName);
        
    hdrSize = 4;
    numLines = hdrSize + length(modFileNames_e) + 1 + length(modFileNames_i);
    outFileLines = {numLines, 1};
    
    outFileLines{1, 1} = '#pragma once';
    outFileLines{2, 1} = '';
    outFileLines{3, 1} = '#include "ModCurrentsBase.h"'; 
    outFileLines{4, 1} = '';
    
    for i = 1 : length(modFileNames_e)
        outFileLines{hdrSize + i, 1} = sprintf('#include "%s.h"', modFileNames_e{i});
    end
    
    outFileLines{i + 1, 1} = '';
    
    for j = 1 : length(modFileNames_i)
        outFileLines{i + 1 + j, 1} = sprintf('#include "%s.h"', modFileNames_i{j});
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
