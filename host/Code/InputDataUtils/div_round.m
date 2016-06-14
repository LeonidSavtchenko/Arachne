function d = div_round( n, m )
%div_round Division and rounding to nearest integer
d = fix(n / m);
rem = n - m * d;
if (m / 2 <= rem)
    d = d + 1;
end;
end

