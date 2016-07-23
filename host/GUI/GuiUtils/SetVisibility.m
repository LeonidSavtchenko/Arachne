function SetVisibility(h, vis)
    if vis
        vis = 'on';
    else
        vis = 'off';
    end
    set(h, 'Visible', vis);
end
