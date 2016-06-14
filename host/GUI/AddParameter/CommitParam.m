function CommitParam(name, value, relPred, valPred, handlers, unit)

    if strcmp(unit, 'tg') || strcmp(unit, 'it')
        caster = @int32; 
    else
        caster = @(x)x; 
    end
   
    global params panIdx
    params{panIdx}{end + 1} = struct('name', name, 'value', value, 'relPred', relPred, 'valPred', valPred, 'handlers', handlers, 'caster', caster);
   
    cmd1 = ['global ', name];
    cmd2 = [name, ' = value;'];
    eval(cmd1);
    eval(cmd2);
    
end