function test_TranslateLineOfCode()

    path = fullfile(cd, '..', 'host', 'Core', 'ModFileUtils');
    addpath(path);

    % Integral numbers
    test('3^4', 'pow((T)3,(T)4)');
    test('3 ^4', 'pow((T)3 ,(T)4)');
    test('-3^ 4', '-pow((T)3, (T)4)');
    test('12 ^ 54', 'pow((T)12 , (T)54)');
    
    % Rational numbers
    test('.1^2.', 'pow((T).1,(T)2.)');
    test('1.^.2', 'pow((T)1.,(T).2)');
    test('3.5^4.6', 'pow((T)3.5,(T)4.6)');

    % Numbers in exponential form
    test('1.2E3^ 4.5e-6', 'pow((T)1.2E3, (T)4.5e-6)');
    test('1.e8^ .4e9', 'pow((T)1.e8, (T).4e9)');
    test('.2e8 ^1e2', 'pow((T).2e8 ,(T)1e2)');
    test('1e4 ^ 2E8', 'pow((T)1e4 , (T)2E8)');

    % Variables
    test('a^b', 'pow(a,b)');
    test('a1 ^b', 'pow(a1 ,b)');
    test('A^ b7', 'pow(A, b7)');
    test('a_a ^ bCd', 'pow(a_a , bCd)');
    test('a1b2 ^ c3b4', 'pow(a1b2 , c3b4)');
    
    % Numbers and variables
    test('1e8^b', 'pow((T)1e8,b)');
    test('2e-9^b', 'pow((T)2e-9,b)');
    test('myVar ^3e-9', 'pow(myVar ,(T)3e-9)');
    test('a ^2e+6', 'pow(a ,(T)2e+6)');
    test('1e+2 ^3E-4', 'pow((T)1e+2 ,(T)3E-4)');
    test('1e2^ a_8', 'pow((T)1e2, a_8)');
    
    % Function calls
    test('a ^ b(3,4)', 'pow(a , b((T)3,(T)4))');
    test('bar()^4', 'pow(bar(),(T)4)');
    test('4^foo()', 'pow((T)4,foo())');
    test('foo(bar()^4) ^ (-1)', 'pow(foo(pow(bar(),(T)4)) , (-(T)1))');
    
    % Expressions in parentheses
    test('(a +2) ^ (b - 6)', 'pow((a +(T)2) , (b - (T)6))');
    test('8.1 ^(14 - a(x))', 'pow((T)8.1 ,((T)14 - a(x)))');
    test('(3 + sqrt(8 -a/b))^ pi', 'pow(((T)3 + sqrt((T)8 -a/b)), pi)');
    
    % Power of power
    test('3 ^ a9 ^ 7', 'pow((T)3 , pow(a9 , (T)7))');
    test('1.2 ^ 2.3E+9 ^ a(1)', 'pow((T)1.2 , pow((T)2.3E+9 , a((T)1)))');
    test('-141.8e-004^ a+18.3E+5 ^ a(.1^2.)', '-pow((T)141.8e-004, a)+pow((T)18.3E+5 , a(pow((T).1,(T)2.)))');
    test('1^2^3^4^5', 'pow((T)1,pow((T)2,pow((T)3,pow((T)4,(T)5))))');
    
    % All in one
    test(' x ^ (q^w12) - x12 * (x + y)  ^ (2 + (x -12.1)/2) / (z + y)', ' pow(x , (pow(q,w12))) - x12 * pow((x + y)  , ((T)2 + (x -(T)12.1)/(T)2)) / (z + y)');
    
    % Comments
    test(': a^b', ': a^b');
    test('2^3 : 4^5', 'pow((T)2,(T)3) : 4^5');
    
    % No numbers
    test('a / b - c', 'a / b - c');

    % No powers
    test('1 + 3 * 2 / sin(x)', '(T)1 + (T)3 * (T)2 / sin(x)');

end

function test(in, expOut)

    actOut = TranslateLineOfCode(in);
    
    disp(['expOut: ', expOut]);
    disp(['actOut: ', actOut]);
        
    if ~strcmp(expOut, actOut)
        assert(false);
    end
    
end