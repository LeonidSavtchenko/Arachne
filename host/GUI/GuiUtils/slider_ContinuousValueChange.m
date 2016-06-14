function slider_ContinuousValueChange(~, ~)

    global hs
    global yPos00 yPos0
    
    yPos0 = yPos00 + get(hs, 'Max') - round(get(hs, 'Value'));
    
    % Gray strips
    AdjustStrips();
    
    UpdateViewControls();
    
end
