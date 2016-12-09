function MaxModelSize(i2eRatio)
%% Count maximum size of the model
    
    global memPerNodeLimit num_e num_i
    
    h0 = 1000;
    eps = 1;
    
    num_e = 0;
    reqMemPerNode = 0;
    while reqMemPerNode < memPerNodeLimit
        x1 = num_e;
        num_e = num_e + h0;
        num_i = i2eRatio * num_e; %#ok<NASGU>
        correct_num();
        reqMemPerNode = CountTotalMemReq();
        x2 = num_e;
    end
    
    while x2 - x1 > eps
        num_e = (x1 + x2) / 2;
        num_i = i2eRatio * num_e; %#ok<NASGU>
        correct_num();
        func = CountTotalMemReq() - memPerNodeLimit;
        if func > 0
            x2 = num_e;
        else
            x1 = num_e;
        end
    end
    
    num_e = floor(x1);
    num_i = floor(i2eRatio * x1);
    correct_num();
    
end


function correct_num()

    global num_e num_i
    global radius_e radius_i v
    global delta_ee delta_ii delta_ei delta_ie max_delay_e max_delay_i
    
    rx = (radius_e + radius_i) / 2 + abs(radius_e - radius_i);
    num_x = (num_e + num_i) / 2 + abs(num_e - num_i);
    delta_ee = round(2 * pi * radius_e / (num_e * v));
    delta_ii = round(2 * pi * radius_i / (num_i * v));
    delta_ei = round(2 * pi * rx / (num_x * v));
    delta_ie = delta_ei;
    
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
    
end