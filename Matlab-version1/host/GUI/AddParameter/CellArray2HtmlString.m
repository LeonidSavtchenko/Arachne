function desc = CellArray2HtmlString(desc)
% Convert cell array to HTML string with <br/> line separator

    assert(iscell(desc));
    desc = strjoin(desc, '<br/>');
    
end