function GenerateCumulativeCppHdrFile(modFileNames_e, modFileNames_i, outDirPath)

    outFileName = 'AllModCurrents.h';
    
    fprintf('    Generating %s ...\n', outFileName);
    
    lenModFileNames_e = length(modFileNames_e);
    lenModFileNames_i = length(modFileNames_i);
    
    outFileLines = {};
    
    outFileLines{end + 1, 1} = '#pragma once';
    outFileLines{end + 1, 1} = '';
    outFileLines{end + 1, 1} = '#include <vector>';
    outFileLines{end + 1, 1} = '#include <memory>';
    outFileLines{end + 1, 1} = '#include <tuple>';
    outFileLines{end + 1, 1} = '';
    outFileLines{end + 1, 1} = '#include "Containers/DistVector.h"';
    outFileLines{end + 1, 1} = '';
 
    for i = 1 : lenModFileNames_e
        outFileLines{end + 1, 1} = sprintf('#include "%s.h"', modFileNames_e{i}); %#ok<AGROW>
    end
    
    outFileLines{end + 1, 1} = '';
    
    for i = 1 : lenModFileNames_i
        outFileLines{end + 1, 1} = sprintf('#include "%s.h"', modFileNames_i{i}); %#ok<AGROW>
    end
    
    outFileLines{end + 1, 1} = '';
    outFileLines = GenerateBaseCppClass(outFileLines);
    outFileLines{end + 1, 1} = '';
    outFileLines = GenerateChildCppClass(outFileLines, modFileNames_e, 'e');
    outFileLines{end + 1, 1} = '';
    outFileLines = GenerateChildCppClass(outFileLines, modFileNames_i, 'i');

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

function outFileLines = GenerateBaseCppClass(outFileLines)
    
    outFileLines{end + 1, 1} = 'template <typename T>';
    outFileLines{end + 1, 1} = 'class AllModCurrents';
    outFileLines{end + 1, 1} = '{';
    outFileLines{end + 1, 1} = '    public:';
    outFileLines{end + 1, 1} = '    AllModCurrents() = default;';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    virtual void SetVoltage(const DistVector<T>& v) { }'; %!!
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    virtual DistVector<T> GetSumCurrent(int length)';
    outFileLines{end + 1, 1} = '    {';
    outFileLines{end + 1, 1} = '        DistVector<T> I(length);';
    outFileLines{end + 1, 1} = '        I.AssignZeros();';
    outFileLines{end + 1, 1} = '        ';
    outFileLines{end + 1, 1} = '        return I;';
    outFileLines{end + 1, 1} = '    }';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    virtual DistVector<T> DoOneStepPart1(const DistVector<T>& v, T dt05)';
    outFileLines{end + 1, 1} = '    {';
    outFileLines{end + 1, 1} = '        DistVector<T> I(v.length);';
    outFileLines{end + 1, 1} = '        I.AssignZeros();';
    outFileLines{end + 1, 1} = '        ';
    outFileLines{end + 1, 1} = '        return I;';
    outFileLines{end + 1, 1} = '    }';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    virtual DistVector<T> DoOneStepPart2(const DistVector<T> &v, const DistVector<T> &v_tmp, T dt05)';
    outFileLines{end + 1, 1} = '    {';
    outFileLines{end + 1, 1} = '        DistVector<T> I(v.length);';
    outFileLines{end + 1, 1} = '        I.AssignZeros();';
    outFileLines{end + 1, 1} = '        ';
    outFileLines{end + 1, 1} = '        return I;';
    outFileLines{end + 1, 1} = '    }';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '};';
end

function outFileLines = GenerateChildCppClass(outFileLines, modFileNames, neuronType)

    outFileLines{end + 1, 1} = 'template <typename T>';
    outFileLines{end + 1, 1} = ['class AllModCurrents_', neuronType, ' : public AllModCurrents<T>'];
    
    if isempty(modFileNames)
        outFileLines{end + 1, 1} = '{';
        outFileLines{end + 1, 1} = 'public:';
        outFileLines{end + 1, 1} = ['    AllModCurrents_', neuronType, '(int num_', neuronType, ') { }'];
        outFileLines{end + 1, 1} = '};';
        return
    end

    lenModFileNames = length(modFileNames);

    outFileLines{end + 1, 1} = '{';
    outFileLines{end + 1, 1} = 'public:';
    
    %************************* Custom constructor *************************%
    outFileLines{end + 1, 1} = ['    AllModCurrents_', neuronType, '(int num_', neuronType, ')'];
    outFileLines{end + 1, 1} = '{';
    outFileLines{end + 1, 1} = '';
    argMakeTuple = '';
    argMakeTuple = [argMakeTuple, 'std::make_shared<DistVector<mod::', modFileNames{1}, '<T>>>(num_', neuronType, ')']; 
    if lenModFileNames > 1
        for i = 2 : lenModFileNames
            argMakeTuple = [argMakeTuple, ', std::make_shared<DistVector<mod::', modFileNames{i}, '<T>>>(num_', neuronType, ')']; %#ok<AGROW>
        end
    end
     
    outFileLines{end + 1, 1} = ['    m_modCurrents = std::make_tuple(', argMakeTuple, ');'];
    outFileLines{end + 1, 1} = '';
    outFileLines{end + 1, 1} = '}';
    outFileLines{end + 1, 1} = '';
    %**********************************************************************%

    %***************************** SetVoltage *****************************%
    outFileLines{end + 1, 1} = 'virtual void SetVoltage(const DistVector<T>& v) override';
    outFileLines{end + 1, 1} = '{';
    outFileLines{end + 1, 1} = '    using namespace DistEnv;';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    int localLength = v.localLength;';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    int myThread = omp_get_thread_num();';
    outFileLines{end + 1, 1} = '    int startIdx = GetThreadChunkStartIdx(localLength, myThread);';
    outFileLines{end + 1, 1} = '    int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['    auto& currModCurrent_', int2str(i - 1), ' = *std::get<', ...
            int2str(i - 1), '>(m_modCurrents);']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '    for (int idx = startIdx; idx < endIdx; idx++)';
    outFileLines{end + 1, 1} = '    {';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['        auto& cModCurr_', int2str(i - 1), ' = currModCurrent_', ...
            int2str(i - 1), '[idx];']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.v = v[idx];']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '    }';
    outFileLines{end + 1, 1} = '}';
    outFileLines{end + 1, 1} = '';
    %**********************************************************************%
    
    %**************************** GetSumCurrent ***************************%
    outFileLines{end + 1, 1} = 'virtual DistVector<T> GetSumCurrent(int length) override';
    outFileLines{end + 1, 1} = '{';
    outFileLines{end + 1, 1} = '    using namespace DistEnv;';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    DistVector<T> I(length);';
    outFileLines{end + 1, 1} = '    I.AssignZeros();';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    int localLength = I.localLength;';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    int myThread = omp_get_thread_num();';
    outFileLines{end + 1, 1} = '    int startIdx = GetThreadChunkStartIdx(localLength, myThread);';
    outFileLines{end + 1, 1} = '    int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);';
    outFileLines{end + 1, 1} = '    ';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['    auto& currModCurrent_', int2str(i - 1),' = *std::get<',...
            int2str(i - 1), '>(m_modCurrents);']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    T I_tmp;';
    outFileLines{end + 1, 1} = '    for (int idx = startIdx; idx < endIdx; idx++)';
    outFileLines{end + 1, 1} = '    {';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['        auto& cModCurr_', int2str(i - 1), ' = currModCurrent_', ...
            int2str(i - 1), '[idx];']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.currents();']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '        ';
    outFileLines{end + 1, 1} = '        I_tmp = T(0);';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['        I_tmp += ', 'cModCurr_', int2str(i - 1), ...
            '.getResCurrent();']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '        I[idx] = I_tmp;';
    outFileLines{end + 1, 1} = '    }';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    return I;';
    outFileLines{end + 1, 1} = '}';
    outFileLines{end + 1, 1} = '';
    %**********************************************************************%
    
    %*************************** DoOneStepPart1 ***************************%
    outFileLines{end + 1, 1} = 'virtual DistVector<T> DoOneStepPart1(const DistVector<T>& v, T dt05) override';
    outFileLines{end + 1, 1} = '{';
    outFileLines{end + 1, 1} = '    using namespace DistEnv;';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    DistVector<T> I(v.length);';
    outFileLines{end + 1, 1} = '    I.AssignZeros();';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    int localLength = I.localLength;';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    int myThread = omp_get_thread_num();';
    outFileLines{end + 1, 1} = '    int startIdx = GetThreadChunkStartIdx(localLength, myThread);';
    outFileLines{end + 1, 1} = '    int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);';
    outFileLines{end + 1, 1} = '';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['    auto& currModCurrent_', int2str(i - 1), ' = *std::get<', ...
            int2str(i - 1), '>(m_modCurrents);']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    T I_tmp;';
    outFileLines{end + 1, 1} = '    for (int idx = startIdx; idx < endIdx; idx++)';
    outFileLines{end + 1, 1} = '    {';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['        auto& cModCurr_', int2str(i - 1), ...
            ' = currModCurrent_', int2str(i - 1), '[idx];']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.v = v[idx];']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.init();']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.states();']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.integrate(dt05);']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.currents();']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '        I_tmp = T(0);';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['        I_tmp += ', 'cModCurr_', int2str(i - 1), ...
            '.getResCurrent();']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '        I[idx] = I_tmp;';
    outFileLines{end + 1, 1} = '    }';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    return I;';
    outFileLines{end + 1, 1} = '}';
    outFileLines{end + 1, 1} = '';
    %**********************************************************************%
    
    %*************************** DoOneStepPart2 ***************************%
    outFileLines{end + 1, 1} = 'virtual DistVector<T> DoOneStepPart2(const DistVector<T> &v, const DistVector<T> &v_tmp, T dt05) override';
    outFileLines{end + 1, 1} = '{';
    outFileLines{end + 1, 1} = '    using namespace DistEnv;';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    DistVector<T> I(v.length);';
    outFileLines{end + 1, 1} = '    I.AssignZeros();';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    int localLength = v.localLength;';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    int myThread = omp_get_thread_num();';
    outFileLines{end + 1, 1} = '    int startIdx = GetThreadChunkStartIdx(localLength, myThread);';
    outFileLines{end + 1, 1} = '    int endIdx = GetThreadChunkStartIdx(localLength, myThread + 1);';
    outFileLines{end + 1, 1} = '    ';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['    auto& currModCurrent_', int2str(i - 1), ' = *std::get<', ...
            int2str(i - 1), '>(m_modCurrents);']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    T I_tmp;';
    outFileLines{end + 1, 1} = '    for (int idx = startIdx; idx < endIdx; idx++)';
    outFileLines{end + 1, 1} = '    {';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['        auto& cModCurr_', int2str(i - 1), ...
            ' = currModCurrent_', int2str(i - 1), '[idx];']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.v = v_tmp[idx];']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.init();']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.states();']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.integrate(dt05);']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.v = v[idx];']; %#ok<AGROW>
        outFileLines{end + 1, 1} = ['        cModCurr_', int2str(i - 1), '.currents();']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '        ';
    outFileLines{end + 1, 1} = '        I_tmp = T(0);';
    for i = 1 : lenModFileNames
        outFileLines{end + 1, 1} = ['        I_tmp += ', 'cModCurr_', int2str(i - 1), ...
            '.getResCurrent();']; %#ok<AGROW>
    end
    outFileLines{end + 1, 1} = '        I[idx] = I_tmp;';
    outFileLines{end + 1, 1} = '    }';
    outFileLines{end + 1, 1} = '    ';
    outFileLines{end + 1, 1} = '    return I;';
    outFileLines{end + 1, 1} = '}';
    %**********************************************************************%
    
    %**************************** Private part ****************************%
    outFileLines{end + 1, 1} = '';
    outFileLines{end + 1, 1} = 'private:';
    outFileLines{end + 1, 1} = ['    const std::size_t sizeCurrents = ', int2str(lenModFileNames), ';'];
    argTuple = '';
    argTuple = [argTuple, 'std::shared_ptr<DistVector<mod::', modFileNames{1}, '<T>>>']; 
    if lenModFileNames > 1
        for i = 2 : lenModFileNames
            argTuple = [argTuple, ', std::shared_ptr<DistVector<mod::', modFileNames{i}, '<T>>>'];  %#ok<AGROW>
        end
    end
    outFileLines{end + 1, 1} = ['    std::tuple<', argTuple,'> m_modCurrents;'];
    outFileLines{end + 1, 1} = '};';
    %**********************************************************************%
end
