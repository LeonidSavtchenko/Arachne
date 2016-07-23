function ModelParamsRemainder()

    global num_e num_i t_final
    global m_steps
    global dt
    global dynamicGTonicGABA
    global enableFreqDelay freqDelay
    global radius_e radius_i v
    global sigma_ee sigma_ei sigma_ie sigma_ii
    global delta_ee delta_ii delta_ei delta_ie max_delay_e max_delay_i
    global enableSTDP
    global imageMode
    global sigmaDivisor_ee sigmaDivisor_ei sigmaDivisor_ie sigmaDivisor_ii
    global importSTDP
    global stdpFile_ee stdpFile_ii stdpFile_ie stdpFile_ei stdp_factor factorNum
    global size_factor t_start_factor t_end_factor
    global stdpMatrix_ee stdpMatrix_ei stdpMatrix_ii stdpMatrix_ie
    global guiType GuiTypes
    
    % number of iterations:
    m_steps = round(t_final / dt);

    % Compute standard deviations for normal distributions
    % (widths of the bell-shaped functions).
    % sigma_xy = max_matrix_dimension / sigmaDivisor_xy.
    sigma_ee = num_e / sigmaDivisor_ee;
    num_max = max(num_e, num_i);
    sigma_ei = num_max / sigmaDivisor_ei;
    sigma_ie = num_max / sigmaDivisor_ie;
    sigma_ii = num_i / sigmaDivisor_ii;

    if guiType ~= GuiTypes.UtilityPlotStdpModels && guiType ~= GuiTypes.UtilityComputeMaxModelSize
        % Compute parameters of presynaptic voltage history matrices
        rx = (radius_e + radius_i) / 2 + abs(radius_e - radius_i);
        num_x = (num_e + num_i) / 2 + abs(num_e - num_i);
        delta_ee = round(2 * pi * radius_e / (num_e * v));
        delta_ii = round(2 * pi * radius_i / (num_i * v));
        delta_ei = round(2 * pi * rx / (num_x * v));
        delta_ie = delta_ei;
        max_delay_e = max(delta_ee, delta_ei) * num_e;
        max_delay_i = max(delta_ie, delta_ii) * num_i;
        
        % Validate parameters of presynaptic voltage history matrices
        if (delta_ee == 0) || (delta_ei == 0) || (delta_ie == 0) || (delta_ii == 0)
            button = questdlg('The set of model parameters is incorrect. The accuracy can be lost. Do you want to continue?', 'Continue operation', 'Yes', 'No', 'No');
            if strcmp(button, 'Yes')
                if delta_ee == 0
                    delta_ee = 1;
                end
                if delta_ei == 0
                    delta_ei = 1;
                end
                if delta_ie == 0
                    delta_ie = 1;
                end
                if delta_ii == 0
                    delta_ii = 1;
                end
                max_delay_e = max(delta_ee, delta_ei) * num_e;
                max_delay_i = max(delta_ie, delta_ii) * num_i;
            elseif strcmp(button, 'No')
                error('The set of model parameters is incorrect.');
            end
        end
    end
    
    if ~enableSTDP
        imageMode = false;
    end

    if guiType ~= GuiTypes.UtilityPlotStdpModels && dynamicGTonicGABA && enableFreqDelay
        freqDelay_iter = round(freqDelay / dt);
        freqDelay = freqDelay_iter;
    end
    
    if importSTDP
        [stdpMatrix_ee, factor_ee, size_factor_ee, t_start_factor_ee, t_end_factor_ee] = ReadFile(stdpFile_ee);
        [stdpMatrix_ei, factor_ei, size_factor_ei, t_start_factor_ei, t_end_factor_ei] = ReadFile(stdpFile_ei);
        [stdpMatrix_ii, factor_ii, size_factor_ii, t_start_factor_ii, t_end_factor_ii] = ReadFile(stdpFile_ii);
        [stdpMatrix_ie, factor_ie, size_factor_ie, t_start_factor_ie, t_end_factor_ie] = ReadFile(stdpFile_ie);
        
        stdp_factor = [factor_ee; factor_ei; factor_ii; factor_ie];
        factorNum = int32(length(stdp_factor));

        size_factor = [size_factor_ee; size_factor_ei; size_factor_ii; size_factor_ie];
        size_factor = int32(size_factor);

        t_start_factor = [t_start_factor_ee; t_start_factor_ei; t_start_factor_ii; t_start_factor_ie];
        t_end_factor = [t_end_factor_ee; t_end_factor_ei; t_end_factor_ii; t_end_factor_ie];
    end
    
    %% Cast to int32 all the scalars of integer nature
    delta_ee = int32(delta_ee);
    delta_ei = int32(delta_ei);
    delta_ie = int32(delta_ie);
    delta_ii = int32(delta_ii);
    max_delay_e = int32(max_delay_e);
    max_delay_i = int32(max_delay_i);
       
end

function [stdpMatrix, factor, numRows, t_start, t_end] = ReadFile(stdpFile)

    stdpMatrix = dlmread(stdpFile);
    
    numRows = size(stdpMatrix, 1);
    numCols = size(stdpMatrix, 2);
    assert(numRows > 1, 'stdpFile_**: The number of rows is less than 2');
    assert(numCols == 2, 'stdpFile_**: The number of columns does not equal 2');
    
    time = stdpMatrix(:, 1);
    
    % Check that 1st column contains equidistant ascending values
    ulp = 100;
    tol = ulp * eps(max(time));
    d = diff(time);
    assert(max(d) - min(d) < tol, 'stdpFile_**: The first column does not contain equidistant values');
    assert(d(1) > 0, 'stdpFile_**: The first column does not contain ascending values');
    
    factor = stdpMatrix(:, 2);
    t_start = time(1);
    t_end = time(end);
    
end