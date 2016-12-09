function valid = valPred_outFileName(outFileName)

    valid = isempty(regexp(outFileName, '[\\/:*?"<>|]', 'once')) && ...
        ~isempty(outFileName) && ...
        length(fullfile(cd, outFileName)) <= 256;
    
end

