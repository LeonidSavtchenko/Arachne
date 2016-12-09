function CastModelParamsRemainder()

    global delta_ee delta_ei delta_ie delta_ii max_delay_e max_delay_i

    %% Cast to int32 all the scalars of integer nature

    delta_ee = int32(delta_ee);
    delta_ei = int32(delta_ei);
    delta_ie = int32(delta_ie);
    delta_ii = int32(delta_ii);
    max_delay_e = int32(max_delay_e);
    max_delay_i = int32(max_delay_i);
   
end