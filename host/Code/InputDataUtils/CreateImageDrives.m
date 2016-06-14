function CreateImageDrives()
%% Create image external drive for e-cells

    global num_e image
    
    maxNumEditableCells = 32 ^ 2;
    
    num_e = double(num_e);
    
    squareSize = ceil(sqrt(num_e));
    
    if num_e <= maxNumEditableCells
        
        if ispc
            question = 'Do you want to draw a new image with Matlab, draw a new image with MS Paint or load an existent image file?';
            choice = questdlg(question, 'Image option', 'Draw with Matlab', 'Draw with MS Paint', 'Load', 'Draw with Matlab');
        else
            question = 'Do you want to draw a new image or to load an existent image file?';
            choice = questdlg(question, 'Image option', 'Draw', 'Load', 'Draw');
        end
        
        assert(~isempty(choice), 'Aborted by user.');
        
        if strcmp(choice, 'Draw with Matlab') || strcmp(choice, 'Draw')
            image = CreateImageDrivesWithMatlab(squareSize);
        else
            if strcmp(choice, 'Draw with MS Paint')
                CreateImageDrivesFileWithMSPaint(squareSize);
            end
            image = LoadImageDrivesFromFile(squareSize);
        end
        
    else
        % Matlab GUI is too slow to edit the image
        
        question = 'Do you want to draw a new image or to load an existent image file?';
        choice = questdlg(question, 'Image option', 'Draw', 'Load', 'Draw');
        
        assert(~isempty(choice), 'Aborted by user.');
        
        if strcmp(choice, 'Draw')
            CreateImageDrivesFileWithMSPaint(squareSize);
        end
        image = LoadImageDrivesFromFile(squareSize);

    end
    
    num_e = int32(num_e);
    
end

function image = CreateImageDrivesWithMatlab(squareSize)
    
    global num_e palette
    
    image = false(num_e, 1);
    totalNum = squareSize ^ 2;
    
    cellSize = 400; % If num_e = 1, then the button has this size 
    xButton = 100;
    yButton = 30;
    distElem = 20;
    windowInd = 80;
    maxNumTitledCells = 23 ^ 2;
    minWindowSizeX = 400;
    minWindowSizeY = 400;
    
    s = get(0, 'ScreenSize');
    xScreen = s(3);
    yScreen = s(4);
    xCellP = (xScreen - 2 * windowInd - 2 * distElem) / squareSize;
    if xCellP < cellSize
        cellSize = floor(xCellP);
    end
    yCellP = (yScreen - 2 * windowInd - 3 * distElem - yButton) / squareSize;
    if yCellP < cellSize
        cellSize = floor(yCellP);
    end
    
    xWindowSize = max(minWindowSizeX, cellSize * squareSize + 2 * distElem);
    yWindowSize = max(minWindowSizeY, cellSize * squareSize + 3 * distElem + yButton);
    xWindowPosition = (xScreen - xWindowSize) / 2;
    yWindowPosition = (yScreen - yWindowSize) / 2;
    distButton = (xWindowSize - 3 * xButton) / 4;
    
    flag = false;
    fd = figure('Name', 'External drives for e-cells', 'Position', [xWindowPosition yWindowPosition xWindowSize yWindowSize], 'NumberTitle', 'off', 'Resize', 'off', 'MenuBar', 'none', 'Color', palette.backgroundColor);
    uicontrol('Style', 'PushButton', 'Tag', 'ButClick', 'Position', [distButton distElem xButton yButton], 'String', 'Click all', 'BackGroundColor', 'white', 'Callback', @ButClickAll);
    uicontrol('Style', 'PushButton', 'Tag', 'ButOK', 'Position', [2 * distButton + xButton distElem xButton yButton], 'String', 'OK', 'BackGroundColor', 'white', 'Callback', @ButCallback);
    uicontrol('Style', 'PushButton', 'Tag', 'ButCancel', 'Position', [3 * distButton + 2 * xButton distElem xButton yButton], 'String', 'Cancel', 'BackGroundColor', 'white', 'Callback', @ButCallback);
    
    % Enabled buttons
    props = struct('Style', 'ToggleButton', 'BackgroundColor', 'white');
    ht = zeros(1, num_e);
    for idx = 1 : num_e
        yCoord = floor((idx - 1) / squareSize);
        xCoord = idx - 1 - squareSize * yCoord;
        xCoord = distElem + cellSize * xCoord;
        yCoord = 2 * distElem + yButton + cellSize * (squareSize - yCoord - 1);
        props.Position = [xCoord yCoord cellSize cellSize];
        if num_e <= maxNumTitledCells
            % Add text to this button
            props.String = num2str(idx);
        end
        props.Callback = {@ClickCell, idx};
        ht(idx) = uicontrol(props);
    end
    
    % Disabled buttons
    props = struct('Style', 'ToggleButton', 'Enable', 'off');
    for idx = (num_e + 1) : totalNum
        yCoord = floor((idx - 1) / squareSize);
        xCoord = idx - 1 - squareSize * yCoord;
        xCoord = distElem + cellSize * xCoord;
        yCoord = 2 * distElem + yButton + cellSize * (squareSize - yCoord - 1);
        props.Position = [xCoord yCoord cellSize cellSize];
        if num_e <= maxNumTitledCells
            % Add text to this button
            props.String = num2str(idx);
        end
        uicontrol(props);
    end
    uiwait(fd);
    assert(flag, 'Aborted by user.');
    
    %% Nested functions
    
    function ButCallback(src, ~)
        if strcmp(get(src, 'Tag'), 'ButOK')
            flag = true;
        end
        delete(gcf);
    end

    function ClickCell(src, ~, idx)
        if get(src, 'Value') == 1
            set(src, 'BackgroundColor', 'black', 'ForegroundColor', 'white');
        else
            set(src, 'BackgroundColor', 'white', 'ForegroundColor', 'black');
        end
        image(idx) = get(src, 'Value');
    end
    
    function ButClickAll(~, ~)
        for idx = 1 : num_e
            if get(ht(idx), 'Value') == 0
                set(ht(idx), 'Value', 1, 'BackgroundColor', 'black', 'ForegroundColor', 'white');
            else
                set(ht(idx), 'Value', 0, 'BackgroundColor', 'white', 'ForegroundColor', 'black');
            end
            image(idx) = get(ht(idx), 'Value');
        end
    end

end

function CreateImageDrivesFileWithMSPaint(squareSize)
    
    global num_e
    
    % Prepare a blank monochrome image of appropriate size with all unused cells indicated
    blankImage = true(squareSize);
    numUnusedCells = squareSize ^ 2 - num_e;
    blankImage(end - numUnusedCells + 1 : end) = false;
    blankImage = blankImage';

    % Save the image and open it for edition with mspaint
    imageFile = 'image.bmp';
    imwrite(blankImage, imageFile, 'bmp');
    cmd = sprintf('mspaint %s &', imageFile);
    system(cmd);
    
end
    
function image = LoadImageDrivesFromFile(squareSize)
    
    global num_e
    
    % Prepare the array of supported file extensions and their descriptions to use as the file filter
    formatStruct = imformats();
    filterSpec = [{formatStruct.ext}; {formatStruct.description}]';
    for i = 1 : size(filterSpec, 1);
        extArray = filterSpec{i, 1};
        filter = ['*.', extArray{1}];
        for j = 2 : size(extArray, 2)
            filter = [filter, '; *.', extArray{j}]; %#ok<AGROW>
        end
        filterSpec{i, 1} = filter;
    end
    filterSpec = [filterSpec; {'*.*', 'All Files'}];
    
    % Select the file with UI
    tit = sprintf('Open a monochromatic image of size %i-by-%i pixels', squareSize, squareSize);
    [name, path, index] = uigetfile(filterSpec, tit);
    if index == 0
        error('Aborted by user.');
    end
    
    % Read and validate the image
    filePath = fullfile(path, name);
    image = imread(filePath);
    imageSize = size(image);
    if length(imageSize) ~= 2
        % Some formats (e.g. JPG) cannot represent monochromatic image accurately
        error('The image format is not supported.');
    end
    msg = sprintf('An image of size %i-by-%i pixels is expected.', squareSize, squareSize);
    assert(all(imageSize == squareSize), msg);
    info = imfinfo(filePath);
    bitDepth = info.BitDepth;
    black = 0;
    white = 2 ^ bitDepth - 1;
    predicate = image == black | image == white;
    assert(all(predicate(:)), 'The image is not monochromatic. Please use only black and white colors.');
    
    % Invert, flatten and cut the image
    image = ~logical(image)';
    image = image(1 : num_e)';

end