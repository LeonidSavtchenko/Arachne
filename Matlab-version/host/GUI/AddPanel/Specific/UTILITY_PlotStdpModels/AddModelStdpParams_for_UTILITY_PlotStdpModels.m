function AddModelStdpParams_for_UTILITY_PlotStdpModels()
%% Add STDP-related parameters
    
    global enableSTDP

    AddPanel('Model (STDP)');
    
    enableSTDP = true;
    
    AddIterCounter('stdpPeriod', 1000, 'How often Hebbian correction is done (in iterations)');

    AddBoolScalar('windowedSTDP', false, 'Whether smooth window (Epsilon) is applied in the analyzed STDP interval');
    
    AddPosRatScalar('epsAt0p5T', 0.9, '', '<html>The value of the Epsilon function at the point of 0.5 * <b><font color="blue">stdpPeriodIter</font></b> * <b><font color="blue">dt</font></b></html>', 'windowedSTDP', 'epsAt0p5T < 1');
    
    AddBoolScalar('importSTDP', false, 'Whether STDP models are imported from text files');
    
    stdpParams = [ ...
%       ee     ei     ie     ii
        0      0      0      0      ;   % Aplus
        1      1      1      3      ;   % S
        0      0      0      0.012  ;   % epsilon
        0      0      1      0      ;   % Aminus
        20     20     20     20     ;   % tau2, ms^-1
        12     12     12     12     ;   % S2
        0      0      0.2    0      ;   % AC
        0.3    0.3   -0.05   0.3    ;   % TimeCosExp
        0.3    0.3   -0.05   0.3    ;   % TimeCos
       -0.1    0.1    0      0      ;   % AS
       -0.2   -0.2   -0.2   -0.2    ;   % TimeSinExp
        0.05   0.05   0.05   0.05   ;   % TimeAbs
        5      5      5      5      ;   % Shift
        10000  10000  10000  10000  ;   % SinAmpl
        0.001  0.001  0.001  0.001  ;   % SinPeriod
        100    100    100    100    ;   % damper
        0.01   0.01   0.01   0.01  ];   % maxAbsdW
    rowNames = {'Aplus', 'S', 'epsilon', 'Aminus', 'tau2, [ms^-1]', 'S2', 'AC', 'TimeCosExp', 'TimeCos', 'AS', 'TimeSinExp', 'TimeAbs', 'Shift', 'SinAmpl', 'SinPeriod', 'damper', 'maxAbsdW'};
    colNames = {'ee', 'ei', 'ie', 'ii'};
    desc = 'Parameters of spike-timing-dependant plasticity (Hebbian correction) for each matrix of synaptic conductance';
    AddFixedSizeMatrix('stdpParams', stdpParams, rowNames, colNames, desc, '~importSTDP', 'valPred_stdpParams(stdpParams)');
    
    desc = 'Name of the file to import STDP model from (%s-interaction)';
    relPred = 'importSTDP';
    
    stdpFile_ee  = '''stdpModel_ee.txt''';
    valPred = 'exist(stdpFile_ee, ''file'')';
    AddStringScalar('stdpFile_ee', stdpFile_ee, sprintf(desc, 'ee'), relPred, valPred); 
        
    stdpFile_ei  = '''stdpModel_ei.txt''';
    valPred = 'exist(stdpFile_ei, ''file'')';
    AddStringScalar('stdpFile_ei', stdpFile_ei, sprintf(desc, 'ei'), relPred, valPred);
        
    stdpFile_ii  = '''stdpModel_ii.txt''';
    valPred = 'exist(stdpFile_ii, ''file'')';
    AddStringScalar('stdpFile_ii', stdpFile_ii, sprintf(desc, 'ii'), relPred, valPred); 
        
    stdpFile_ie  = '''stdpModel_ie.txt''';
    valPred = 'exist(stdpFile_ie, ''file'')';
    AddStringScalar('stdpFile_ie', stdpFile_ie, sprintf(desc, 'ie'), relPred, valPred); 

end
