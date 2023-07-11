% Define the rules for S.
s --> [].
s --> f.
s --> t, n, t.

% Extra rule to deal with brackets inside input string.
s --> [S], { parse(S) }, s.

% Define the rules for F.
f --> [if], b, [then], ['{'], s, ['}'].
f --> [if], b, [then], ['{'], s, ['}'], [else], ['{'], s, ['}'].

% Define the rules for B.
b --> ['('], t, e, t, [')'].

% Define the rules for T.
t --> [x].
t --> [y].
t --> [0].

% Define the rules for E.
e --> [>].
e --> [<].

% Define the rules for N.
n --> [+].
n --> [-].
n --> [=].

% Define the function to actually start parsing a string.
parse(S) :- s(S, []), !.

% **************************** QUERY TO GENERATE EXAMPLE STRINGS ****************************
% To Run: Execute the query "gen_strings(X, Y, Z)."
% Define function to generate a string that adheres to the grammar rules.
gen_strings(S1, S2, S3) :-

    % Specify some basic requirements for the first string, and then generate it.
    member(if, S1),
    s(S1, []),

    % Specify some basic requirements for the second string, and then generate it.
    member([x, +, y], S2),
    s(S2, []),

    % Specify some basic requirements for the third string, and then generate it.
    member(else, S3),
    s(S3, []), !.