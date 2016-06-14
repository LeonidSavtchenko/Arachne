function Layout()
%% Initialize constants for GUI layout

    global layout
    
    layout.xMargin0 = 10;   % The distance "window border - panel border"
    layout.xMargin1 = 10;   % The distance "panel border - param name text"
    layout.xMargin2 = 5;    % The distance "param name text - value control"
    layout.xMargin3 = 10;   % The distance "value edit box - unit text"
    
    layout.yMargin0 = 10;   % The distance between two controls
    
    layout.rgWidth = 126;   % Radiogroup panel width
    layout.nameWidth = 135; % Param name width
    
    layout.nameTableWidth = 21;	% The width of the first column of the table
 
    layout.ebWidth = 200;   % Param editbox width
    layout.unitWidth = 95;  % Unit text width
    
    layout.yStep = 30;      % Y-step for controls placement
    
    layout.rbHeight = 23;   % Radiobutton height
    layout.pbWidth = 69;    % Pushbutton width
    layout.pbHeight = 22;   % Pushbutton height
    layout.bsHeight = 2;    % Height of blank strips
    layout.sWidth = 17;     % Slider width
    layout.ebHeight = 23;   % Editbox height
    layout.tHeight = 22;    % Text label height
    layout.cbHeight = 23;   % Checkbox height
    layout.pmHeight = 20;   % Popup menu height
    layout.taHeight = 150;  % Text area width
    layout.trHeight = 19.1; % Table row height
    layout.tcWidth = 89.3;  % Table column width
   
    layout.ebYMargin = 4;   % Additional space between editbox (checkbox) and controls
    layout.pmYMargin = 7;   % Additional space between popup menu and controls
    layout.tYMargin = 7;    % Additional space between table and controls

end