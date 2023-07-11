% Define the base case when both lists are empty.
addlist([], [], []) :- !.

% Define the case when only the first list is empty.
addlist([], [S|T], [V|W]) :-

    % Since the first list is empty we add 0 to the second list.
    V is 0 + S,

    % Recursively call the function for the next values.
    addlist([], T, W), !.

% Define the case when only the second list is empty.
addlist([F|G], [], [V|W]) :-

    % Since the second list is empty we add 0 to the first list.
    V is F + 0,

    % Recursively call the function for the next values.
    addlist(G, [], W), !.

% Define the case when neither list is empty.
addlist([F|G], [S|T], [V|W]) :-

    % Add the values from both lists together.
    V is F + S,

    % Recursively call the function for the next values.
    addlist(G, T, W), !.