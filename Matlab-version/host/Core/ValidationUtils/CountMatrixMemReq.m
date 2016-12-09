function reqMemPerMatrix = CountMatrixMemReq(numRows, numCols, np_worst, elemSize, scmType, sigma, w_max, suf)
%% This function counts memory required for synaptic conductance matrix on one node of the cluster (in MB).
%  The last three parameters are used for sparse matrix only.

    global ScmTypes enableSTDP saveIntermMat gatherSCM

    if scmType ~= ScmTypes.HstSparse && scmType ~= ScmTypes.KrnSparse
        numElem = numRows * numCols;
    else
        numElem = EstimateSparseMatrixNNZ(numRows, numCols, sigma, w_max, suf);
    end
    
    switch scmType
        case {ScmTypes.AllZeros, ScmTypes.AllEqual, ScmTypes.KrnInPlace}
            % Allocated memory is insignificant. Rough estimate
            reqMemPerMatrix = 0;
        case ScmTypes.HstDense
            % The master node is impacted the most
            reqMemPerMatrix = numElem * elemSize * (np_worst + 1) / np_worst / 2 ^ 20;
        case ScmTypes.HstSparse
            % The master node is impacted the most
            elemSize = 8;   % Remark: Matlab does not support single-precision sparse matrix
            intSize1 = 8;   % Matlab format
            intSize2 = 4;   % Our format
            reqMemPerMatrix = LocalSparseMatrixMemReq(numElem, elemSize, numCols, intSize1) + LocalSparseMatrixMemReq(numElem, elemSize, numCols, intSize2) / np_worst;
        case ScmTypes.KrnDense
            reqMemPerMatrix = numElem * elemSize / np_worst / 2 ^ 20;
            if (enableSTDP && saveIntermMat) || gatherSCM
                reqMemPerMatrix = reqMemPerMatrix * (np_worst + 1);
            end
        case ScmTypes.KrnSparse
            intSize = 4;
            reqMemPerMatrix = LocalSparseMatrixMemReq(numElem, elemSize, numCols, intSize) / np_worst;
        case ScmTypes.KrnOneBit
            blockSize = 64;
            intSize = 8;
            reqMemPerMatrix = numElem / blockSize * intSize / np_worst / 2 ^ 20;
        otherwise
            error('Bad scmType specified.');
    end

end

function reqMemPerMatrix = LocalSparseMatrixMemReq(numElem, elemSize, numCols, intSize)
    reqMemPerMatrix = (numElem * (elemSize + intSize) + (numCols + 1) * intSize) / 2 ^ 20;
end