function AddFixedSizeMatrix(name, values, rowNames, colNames, desc,   relPred, valPred)

    global maxLengthName
    global layout
    global panIdx params
    
    maxLengthName = 15;
    
    rowNames = CorrectLengthName(rowNames);
       
    if nargin < 6
        relPred = 'true';
    end
    
    % Extend the validation expression
    valPred_ = sprintf('all(~isnan(%s(:))) && all(~isinf(%s(:)))', name, name);
    if nargin == 7
        valPred_ = [valPred_, ' && ', valPred];
    end
    
    % The name
    [handlers, xPos] = CreateParamNameText(name);
    
    % Prepare tooltip
    if iscell(desc)
        desc = ['<html>', CellArray2HtmlString(desc), '</html>'];
    end
    
    % The control itself
    xPos = xPos + layout.xMargin2;
    parIdx = length(params{panIdx}) + 1;
    numDataRows = size(values, 1);
    numDataCols = size(values, 2);
    w = layout.tcHdrWidth + numDataCols * layout.tcWidth;
    h = layout.trHdrHeight + numDataRows * layout.trHeight;
    handlers(end + 1) = uitable('Units', 'pixels', ...
                                'Position', [xPos, 0, w, h], ...
                                'RowName', rowNames, ...
                                'ColumnName', colNames, ...
                                'Data', values, ...
                                'ColumnEditable', true(1, numDataCols), ...
                                'UserData', [panIdx, parIdx], ...
                                'CellEditCallback', @generic_Callback, ...
                                'TooltipString', desc);
    
    CommitParam(name, values, relPred, valPred_, handlers, '');
    
end

function cellNames = CorrectLengthName( cellNames )

    global maxLengthName
    for i = 1 : length(cellNames)
        currName = char(cellNames(i));
        while length(currName) < maxLengthName
            currName = [currName, ' '];
        end
      
        cellNames{i} = currName;
    end
end

