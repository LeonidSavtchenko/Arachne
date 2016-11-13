function UpdateViewControls()
    
    global hf panIdx params customVars
    global yPos yPos0 layout palette
    global invalidParams
    
    global guiType GuiTypes
 
    % Referred from relevancy prediates
    global startFromScratch SclModels ScmTypes %#ok<NUSED>
    global backgroundMode %#ok<NUSED>   For background process monitoring
    
    % Add all variables from customVars structure to the current workspace
    if guiType == GuiTypes.StartFromScratch
        fields = fieldnames(customVars);
        for varIdx = 1 : length(fields)
            cmd = sprintf('%s = customVars.%s;', fields{varIdx}, fields{varIdx});
            eval(cmd);
        end
    end
    
    pos = get(hf, 'Position');
    winWidth = pos(3);
    
    % Loop by panels
    for panIdx_ = 1 : length(params)
        % Loop by all parameters of this panel
        for parIdx = 1 : length(params{panIdx_})
            value = params{panIdx_}{parIdx}.value;
            
            handlers = params{panIdx_}{parIdx}.handlers;
            try
                style = get(handlers(2), 'Style');
            catch
                style = 'uitable';
            end
            if strcmp(style, 'edit')
                min = get(handlers(2), 'Min');
                max = get(handlers(2), 'Max');
            else
                max = 0;
                min = 0;
            end
            if max - min > 1
                % Text area is evaluated elsewhere
            elseif ischar(value)
                try
                    value = eval(value);
                catch
                    value = nan;
                end
            end
            Assign(params{panIdx_}{parIdx}.name, value);
        end
    end
    
    yPos = yPos0;
    
    invalidParams = {};
    
    % Loop by panels
    for panIdx_ = 1 : length(params)
        
        % Loop by all parameters of this panel
        for parIdx = 1 : length(params{panIdx_})
            
            relPred = params{panIdx_}{parIdx}.relPred;
            if ~islogical(relPred)
                rel = eval(relPred);
            else
                rel = relPred;
            end
                
            if panIdx_ == panIdx
                % This is the active panel
                vis = rel;
            else
                % This is a hidden panel
                vis = false;
            end
            
            % Update control visibility
            handlers = params{panIdx_}{parIdx}.handlers;
            try
                style = get(handlers(2), 'Style');
            catch
                style = 'uitable';
            end
            for j = 1 : length(handlers)
                handler = handlers(j);
                pos = get(handler, 'Position');
                pos(2) = yPos;
                if j == 1 || j == 3
                    % Name or Unit
                    if strcmp(style, 'checkbox') || strcmp(style, 'edit')
                        pos(2) = pos(2) - layout.ebYMargin;
                    elseif strcmp(style, 'popupmenu')
                        pos(2) = pos(2) - layout.pmYMargin;
                    end
                elseif j == 2 && strcmp(style, 'uitable')
                    % Table
                    tablePos = get(handlers(2), 'Position');
                    pos(2) = pos(2) - tablePos(4) + layout.yStep - layout.tYMargin;
                end
                    
                set(handler, 'Position', pos);
                SetVisibility(handler, vis);

                if j == 2 && (strcmp(style, 'edit') || strcmp(style, 'uitable'))
                    % The control itself
                    
                    % Evaluate the validation predicate
                    valPred = params{panIdx_}{parIdx}.valPred;
                    try
                        val = eval(valPred);
                    catch
                        val = false;
                    end
                    
                    if vis
                        if ~strcmp(style, 'uitable')
                            if val
                                color = palette.validColor;
                            else
                                color = palette.invalidColor;
                            end
                        else
                            if val
                                color = palette.tableValidColor;
                            else
                                color = palette.tableInvalidColor;
                            end
                        end
                        set(handler, 'BackgroundColor', color);
                    end
                    if rel && ~val
                        invalidParams{end + 1} = params{panIdx_}{parIdx}.name; %#ok<AGROW>
                    end
                end
            end
            
            if strcmp(style, 'uitable') 
                pos = get(handlers(2), 'Position');
                numCols = length(get(handlers(2), 'ColumnName')) + 1;
                pos(3) = numCols * layout.tcWidth + layout.nameTableWidth; 
                numRows = length(get(handlers(2), 'Data')) + 1;  % include header
                h = numRows * layout.trHeight;
                pos(2) = pos(2) + pos(4) - h;
                pos(4) = h - Translation(numRows); 
                set(handlers(2), 'Position', pos);
            end
            
            if vis && strcmp(style, 'edit')
                min = get(handlers(2), 'Min');
                max = get(handlers(2), 'Max');
                pos = get(handlers(2), 'Position');
                if max - min > 1
                    if panIdx_ == 1
                        % Adjust position and size of the multiline edit control
                        % to fill all available space on the panel
                        pos(2) = layout.bsHeight + layout.pbHeight + 5 * layout.yMargin0;
                        pos(4) = yPos - pos(2) + layout.ebHeight - layout.ebYMargin;
                    else
                        pos(2) = pos(2) - pos(4) + layout.ebHeight;
                    end
                    pos(3) = winWidth - pos(1) - layout.sWidth - 2 * layout.xMargin0;
                    set(handlers(2), 'Position', pos); 
                    yPos = yPos - pos(4) + layout.ebHeight;
                end
            end
            
            if vis && strcmp(style, 'uitable')
                % Adjust size of the uitable to fit the panel size
                pos = get(handlers(2), 'Position');
                maxX = winWidth - layout.sWidth - 3 * layout.xMargin0;
                isTooWide = false;  % if width already fits the panel, height will not be adjusted
                if pos(1) + pos(3) > maxX
                    pos(3) = maxX - pos(1);
                    isTooWide = true;
                end
                minY = layout.bsHeight + layout.pbHeight + 3 * layout.yMargin0;
                if isTooWide && (pos(2) + pos(4) > minY) && (pos(2) < minY)
                    pos(2) = minY;
                    pos(4) = yPos + layout.ebHeight - pos(2);
                end
                set(handlers(2), 'Position', pos);
            end
            
            if vis && strcmp(style, 'pushbutton')
                pos = get(handlers(2), 'Position');
                pos(2) = pos(2) + layout.mspbYMargin;
                set(handlers(2), 'Position', pos);
            end
            
            if vis
                if ~strcmp(style, 'uitable')
                    yPos = yPos - layout.yStep;
                else
                    tableSize = get(handlers(2), 'Position');
                    yPos = yPos - tableSize(4);
                end
            end
        end
    end
end

function Assign(name, value)
    assignin('caller', name, value);
    
    cmd1 = ['global ', name];
    cmd2 = [name, ' = value;'];
    eval(cmd1);
    eval(cmd2);
end

function val = Translation(numRows)

    if numRows < 6
        val = 0;
    else
        val = numRows ^ 1.75 / 18.75; 
    end

end