function line = TranslateLineOfCode(line)
%% Translate one line of code from PROCEDURE or FUNCTION block of MOD file 
%  so that we can put in into the autogenerated CPP file.

    % If this line has a comment at the end, remove the comment
    idx = strfind(line, ':');
    if ~isempty(idx)
        firstColonIdx = idx(1);
    else
        firstColonIdx = length(line) + 1;
    end
    substr = line(1 : firstColonIdx - 1);
    
    % Replace all "^" operators with "pow" function calls
    %substr = ReplaceCaretsWithPows(substr);
    
    % Maybe it's a temporary solution %!!!
    substr = ReplaceToPow(substr);
    
    % Add "(T)" in front of all numbers
    substr = AddCastOfNumbersToT(substr);
    
    % Revert "(T)" in square brackets "[]"
    substr = RevertCastOfNumbersToT(substr);
    
    % Restore the comment
    line = [substr, line(firstColonIdx : end)];
    
end

function line = ReplaceCaretsWithPows(line)
%% Replace all "^" operators with "pow" function calls as following:
%  "... {first_operand}^{second_operand} ..." -> "... pow({first_operand},{second_operand}) ..."

    % One-time search for all carets and loop by them in reversed order is not sufficient,
    % because replacement of a right caret can change index of
    % a left caret in some cases, e.g. "(a^b)^c".
    % So we have to search for the last caret explicitly each time.
    caretIdxs = strfind(line, '^');
    while ~isempty(caretIdxs)
        
        caretIdx = caretIdxs(end);
        
        % Get the substring at right of the caret
        rightSubstr = line(caretIdx + 1 : end);
        
        % Find end index of the "^" second operand
        rightEndIdx = FindSecondOperandEndIdx(rightSubstr);
        assert(rightEndIdx ~= -1, 'Failed to find the caret second operand end');
        rightEndIdx = caretIdx + rightEndIdx;
        
        % Get the substring at left of the caret in reversed direction
        revLeftSubstr = line(caretIdx - 1 : -1 : 1);
        
        % Find end index of the "^" first operand in the reversed substring
        revLeftStartIdx = FindRevFirstOperandStartIdx(revLeftSubstr);
        assert(revLeftStartIdx ~= -1, 'Failed to find the caret first operand start');
        leftStartIdx = caretIdx - revLeftStartIdx;
        
        assert(line(caretIdx) == '^');
        
        % Replace "^" operator with "pow" function call
        line = [ ...
            line(1 : leftStartIdx - 1), ...
            'pow(', ...
            line(leftStartIdx : caretIdx - 1), ...
            ',', ...
            line(caretIdx + 1 : rightEndIdx), ...
            ')', ...
            line(rightEndIdx + 1 : end)];

        caretIdxs = strfind(line, '^');
    end

end

function endIdx = FindSecondOperandEndIdx(rightSubstr)
%% Find end index of the second operand of "^" operator.
%  The input string is the right-hand part of "^".

    notWhitespace = '\S';
    
    % Skip all whitespaces after the caret
    startIdx = regexp(rightSubstr, notWhitespace, 'once');
    if isempty(startIdx)
        endIdx = -1;
        return
    end

    endIdx = TryParseAsVariableOrFuncCallNotReversed(rightSubstr, startIdx);
    if endIdx ~= -1
        return
    end
    
    endIdx = TryParseAsNumber(rightSubstr, startIdx, false);
    if endIdx ~= -1
        return
    end
    
    endIdx = TryParseAsParenthesizedExpr(rightSubstr, startIdx, false);
    if endIdx ~= -1
        return
    end
    
end

function revStartIdx = FindRevFirstOperandStartIdx(revLeftSubstr)
%% Find start index of the first operand of "^" operator.
%  The input string is the left-hand part of "^" in reversed direction.

    notWhitespace = '\S';
    
    % Skip all whitespaces after the caret in the reversed substring
    revEndIdx = regexp(revLeftSubstr, notWhitespace, 'once');
    if isempty(revEndIdx)
        revStartIdx = -1;
        return
    end

    revStartIdx = TryParseAsVariableOrFuncCallOrParenthesizedExprReversed(revLeftSubstr, revEndIdx);
    if revStartIdx ~= -1
        return
    end
    
    revStartIdx = TryParseAsNumber(revLeftSubstr, revEndIdx, true);
    if revStartIdx ~= -1
        return
    end
    
end

function endIdx = TryParseAsNumber(str, startIdx, isReversed)
%% Try to parse the string as a number

    % Keep these expressions in sync with those ones defined in "AddCastOfNumbersToT"
    c = '(([0-9]+)?\.)?';
    d = '[0-9]+';
    e = '(\.)?';
    f1 = '([Ee]([\+\-])?[0-9]+)?';
    f2 = '([0-9]+([\+\-])?[Ee])?';
    if ~isReversed
        number = [c, d, e, f1];
    else
        number = [f2, c, d, e];
    end
    
    substr = str(startIdx : end);
    [startIdx_, endIdx] = regexp(substr, number, 'once');
    if ~isempty(startIdx_) && startIdx_ == 1
        endIdx = startIdx - 1 + endIdx;
    else
        endIdx = -1;
    end
    
end

function endIdx = TryParseAsVariableOrFuncCallNotReversed(str, startIdx)
%% Try to parse the string as a variable or function call

    % Keep these expressions in sync with those ones defined in "TryParseAsVariableOrFuncNameReversed"
    b = '[A-Z_a-z]';
    c = '([\dA-Z_a-z]+)?';
    
    varOrFuncName = [b, c];

    substr = str(startIdx : end);
    [varOrFuncNameStartIdx, varOrFuncNameEndIdx] = regexp(substr, varOrFuncName, 'once');
    if isempty(varOrFuncNameStartIdx) || varOrFuncNameStartIdx ~= 1
        endIdx = -1;
        return
    end

    endIdx = startIdx - 1 + varOrFuncNameEndIdx;

    % Check if this is a function call
    if endIdx + 1 < length(str)
        closingParenthesisIdx = TryParseAsParenthesizedExpr(str, endIdx + 1, false);
        if closingParenthesisIdx ~= -1
            endIdx = closingParenthesisIdx;
        end
    end
    
end

function endIdx = TryParseAsVariableOrFuncCallOrParenthesizedExprReversed(str, startIdx)
%% Try to parse the string as a variable or function call or parenthesized expression in reversed direction

    % Check if this is a function call
    openingParenthesisIdx = TryParseAsParenthesizedExpr(str, startIdx, true);
    if openingParenthesisIdx ~= -1
        endIdx = openingParenthesisIdx;

        revFuncNameStartIdx = TryParseAsVariableOrFuncNameReversed(str, endIdx + 1);
        if revFuncNameStartIdx ~= -1
            % This is a function call
            endIdx = revFuncNameStartIdx;
        end
        return
    end

    % Check if this is a variable
    endIdx = TryParseAsVariableOrFuncNameReversed(str, startIdx);
    
end

function endIdx = TryParseAsVariableOrFuncNameReversed(str, startIdx)
%% Try to parse the string as a variable or function name in reversed direction

    % Keep these expressions in sync with those ones defined in "TryParseAsVariableOrFuncCallNotReversed"
    a2 = '(?![\.\d])';
    b = '[A-Z_a-z]';
    c = '([\dA-Z_a-z]+)?';

    revVarOrFuncName = [c, b, a2];

    substr = str(startIdx : end);
    [revVarOrFuncNameEndIdx, revVarOrFuncNameStartIdx] = regexp(substr, revVarOrFuncName, 'once');
    if ~isempty(revVarOrFuncNameEndIdx) && revVarOrFuncNameEndIdx == 1
        % This is a variable of function name
        endIdx = startIdx - 1 + revVarOrFuncNameStartIdx;
    else
        endIdx = -1;
    end
    
end

function endIdx = TryParseAsParenthesizedExpr(str, startIdx, isReversed)
%% Try to parse the string as an expression enclosed by parentheses

    if ~isReversed
        openingParenthesis = '(';
        closingParenthesis = ')';
    else
        openingParenthesis = ')';
        closingParenthesis = '(';
    end
    
    endIdx = -1;
    
    if str(startIdx) ~= openingParenthesis
        return
    end
        
    % The number of opened parentheses
    counter = 1;
    
    for idx = startIdx  + 1 : length(str)
        if str(idx) == closingParenthesis
            counter = counter - 1;
            if counter == 0
                endIdx = idx;
                return
            end
        elseif str(idx) == openingParenthesis
            counter = counter + 1;
        end
    end
    
end

function line = AddCastOfNumbersToT(line)
%% Add "(T)" in front of all numbers

    % Keep these expressions in sync with those ones defined in "TryParseAsNumber"
    a = '(?<![A-Z_a-z])';
    b = '(?<![0-9])';
    c = '(([0-9]+)?\.)?';
    d = '[0-9]+';
    e = '(\.)?';
    f = '([Ee]([\+\-])?[0-9]+)?';
    
    number = [a, b, c, d, e, f];
    subst = '(T)$&';
    line = regexprep(line, number, subst);

    errorInExponent = '(?<=\d\.[Ee][\+\-])(\(T\)){1}';
    subst = '';
    line = regexprep(line, errorInExponent, subst);
    
end

function line = RevertCastOfNumbersToT(line)
%% Revert "(T)" in square brackets "[]"
    
    rightBracketIdx = strfind(line, ']');
    
    if isempty(rightBracketIdx)
        return
    end
    
    if length(rightBracketIdx) == 1
        leftBracketIdx = strfind(line, '[');
        assert(~isempty(leftBracketIdx), 'Failed to find the left bracket "["');
        leftBracketIdx = leftBracketIdx(1);
        
        if leftBracketIdx == 1
            leftSubLine = '';
        else
           leftSubLine = line(1 : leftBracketIdx - 1);
        end
        
        centerSubLine = regexprep(line(leftBracketIdx : rightBracketIdx), '\(T\)', '');
        
        if rightBracketIdx == length(line)
            rightSubLine = '';
        else
           rightSubLine = line(rightBracketIdx + 1 : end);
        end
        
        line = [leftSubLine, centerSubLine, rightSubLine];
        
    else
        rightBracketIdx = rightBracketIdx(1);
        headLine = line(1 : rightBracketIdx);
        tailLine = line(rightBracketIdx + 1 : end);
        
        leftBracketIdx = strfind(headLine, '[');
        assert(~isempty(leftBracketIdx), 'Failed to find the left bracket "["');
        leftBracketIdx = leftBracketIdx(end);
        
        if leftBracketIdx == 1
            newHead = regexprep(headLine(leftBracketIdx : end), '\(T\)', '');
        else
            newHead = [headLine(1 : leftBracketIdx - 1), ...
                regexprep(headLine(leftBracketIdx : end), '\(T\)', '')];
        end
        
        line = [newHead, RevertCastOfNumbersToT(tailLine)];
    end
end

function line = ReplaceToPow(line)
    
    while strfind(line, '^')
        line = FirstReplaceToPow(line);
    end
end

function line = FirstReplaceToPow(line)
    
    keyWords = '=+-*/';
    
    idx = strfind(line, '^');
    idx = idx(1);
    
    leftPos = -1;
    leftScopeSum = 0;
    rightScopeSum = 0;
    for i = 1 : idx - 1
        cChar = line(idx - i);
        if cChar == ')'
            rightScopeSum = rightScopeSum + 1;
        elseif cChar == '('
            leftScopeSum = leftScopeSum + 1;
            if leftScopeSum == rightScopeSum 
                leftPos = idx - i;
                break 
            elseif rightScopeSum == 0
                leftPos = idx - i + 1;
                break 
            end
        elseif leftScopeSum == rightScopeSum && ~isempty(strfind(keyWords, cChar))
            leftPos = idx - i + 1;
            break
        elseif i == idx - 1
            leftPos = 1;
        end
    end
    
    assert(leftPos > 0, 'Bad "leftPos" index');
    leftArg = line(leftPos : idx - 1);
    
    lenLine = length(line);
    rightPos = -1;
    leftScopeSum = 0;
    rightScopeSum = 0;
    for i = idx + 1 : lenLine
        cChar = line(i);
        if cChar == '('
            leftScopeSum = leftScopeSum + 1;
        elseif cChar == ')'  
            rightScopeSum = rightScopeSum + 1;
            if leftScopeSum == rightScopeSum
                rightPos = i;
                break
            elseif leftScopeSum == 0
                rightPos = i - 1;
                break
            end
          
        elseif leftScopeSum == rightScopeSum && ~isempty(strfind(keyWords, cChar))
            rightPos = i - 1;
            break
        elseif i == lenLine
            rightPos = lenLine;
        end
    end
    
    assert(rightPos <= lenLine, 'Bad "rightPos" index');
    rightArg = line(idx + 1 : rightPos);
    
    leftSubLine = '';
    if leftPos > 1
        leftSubLine = line(1 : leftPos - 1);
    end
    
    rightSubLine = '';
    if rightPos < lenLine
        rightSubLine = line(rightPos + 1 : end);
    end
    
    line = [leftSubLine, 'pow(', leftArg, ', ', rightArg, ')', rightSubLine];   
end


