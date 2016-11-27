function GenerateCumulativeCppHdrFile(modFileNames_e, modFileNames_i, outDirPath)

    outFileName = 'AllModCurrents.h';
    
    fprintf('Generating %s ...\n', outFileName);
    
    lenModFileNames_e = length(modFileNames_e);
    lenModFileNames_i = length(modFileNames_i);
    
    hdrSize = 20;
    numLines = hdrSize + lenModFileNames_e + 1 + lenModFileNames_i;
    outFileLines = {numLines, 1};
    
    outFileLines{1, 1} = '#pragma once';
    outFileLines{2, 1} = '';
    
    for i = 1 : lenModFileNames_e
        outFileLines{2 + i, 1} = sprintf('#include "%s.h"', modFileNames_e{i});
    end
    
    outFileLines{lenModFileNames_e + 3, 1} = '';
    
    for j = 1 : lenModFileNames_i
        outFileLines{lenModFileNames_e + 3 + j, 1} = sprintf('#include "%s.h"', modFileNames_i{j});
    end
     
    currIdx = lenModFileNames_e + 3 + lenModFileNames_i;
    
    outFileLines{currIdx + 1, 1} = 'template<typename ... Types>';
    outFileLines{currIdx + 2, 1} = 'class VariadicTemplate : public Types ...'; 
    outFileLines{currIdx + 3, 1} = '{';
    outFileLines{currIdx + 4, 1} = 'public:';
    outFileLines{currIdx + 5, 1} = '';
    outFileLines{currIdx + 6, 1} = 'VariadicTemplate(... ) : Types()... {}';
    outFileLines{currIdx + 7, 1} = '';
    outFileLines{currIdx + 8, 1} = '};';
    outFileLines{currIdx + 9, 1} = '';
   
    if ~isempty(modFileNames_e)
        typeNamesStr_e = '';
       
        if(lenModFileNames_e == 1)
            typeNamesStr_e = [typeNamesStr_e, 'mod::', modFileNames_e{1},'<T>'];
        else
            typeNamesStr_e = [typeNamesStr_e, 'mod::', modFileNames_e{1},'<T>'];
            
            for i = 2 : length(modFileNames_e)
                typeNamesStr_e = [typeNamesStr_e, ', ', 'mod::', modFileNames_e{i},'<T>'];  %#ok<AGROW>
            end
        end
        
        outFileLines{currIdx + 10, 1} = ['template<typename T> using t_AllModCurrents_e = VariadicTemplate<', ...
            typeNamesStr_e, '>;'];
        
        outFileLines{currIdx + 11, 1} = 'template<typename T>';
        outFileLines{currIdx + 12, 1} = 'class AllModCurrents_e : public t_AllModCurrents_e<T>';
        outFileLines{currIdx + 13, 1} = '{};';
    else
        outFileLines{currIdx + 10, 1} = '';
        outFileLines{currIdx + 11, 1} = '';
        outFileLines{currIdx + 12, 1} = '';
        outFileLines{currIdx + 13, 1} = '';
    end
    
    outFileLines{currIdx + 14, 1} = '';

    if ~isempty(modFileNames_i)
        typeNamesStr_i = '';
      
        if(lenModFileNames_i == 1)
            typeNamesStr_i = [typeNamesStr_i, 'mod::', modFileNames_i{1},'<T>'];
        else
            typeNamesStr_i = [typeNamesStr_i, 'mod::', modFileNames_i{1},'<T>'];
            
            for i = 2 : length(modFileNames_i)
                typeNamesStr_i = [', ', typeNamesStr_i, 'mod::', modFileNames_i{i},'<T>'];  %#ok<AGROW>
            end
        end
        outFileLines{currIdx + 15, 1} = ['template<typename T> using t_AllModCurrents_i = VariadicTemplate<', ...
            typeNamesStr_i, '>;'];
        
        outFileLines{currIdx + 16, 1} = 'template<typename T>';
        outFileLines{currIdx + 17, 1} = 'class AllModCurrents_i : public t_AllModCurrents_i<T>';
        outFileLines{currIdx + 18, 1} = '{};';
    else
        outFileLines{currIdx + 15, 1} = '';
        outFileLines{currIdx + 16, 1} = '';
        outFileLines{currIdx + 17, 1} = '';
        outFileLines{currIdx + 18, 1} = '';
    end
    
    outFileLines{currIdx + 19, 1} = '';

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
