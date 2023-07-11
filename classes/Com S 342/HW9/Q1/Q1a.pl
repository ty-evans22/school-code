% Define the function to reverse a list.
% Define the base case for an empty list.
reverse([], L2, L2).

% Define the recursive helper function to reverse the list.
reverse([H|T], L2, R) :-

    % Place the first element of the first list as the first element
    % of the second argument and call the function recursively.
    reverse(T, [H|L2], R).

% Define the function to reverse a list.
reverse(L, R) :-

    % Call the helper function to reverse the list.
    reverse(L, [], R).

% Define the base case when the number is <= 2.
fib_builder(0, []).
fib_builder(1, [0]).
fib_builder(2, [1, 0]).

% Define the recursive helper function to return the list
% of Fibonacci numbers.
fib_builder(N, [R, X, Y|Z]) :-

    % Check if the number is > 2.
    N > 2,

    % Compute the next number in the sequence.
    N1 is N - 1,

    % Call the function recursively w/ the next value in the sequence
    % and append the current value in the list.
    fib_builder(N1, [X, Y|Z]),

    % Compute the sum of the two consecutive values to create the
    % Fibonacci series.
    R is X + Y.

% Define the function to return a list of Fibonacci numbers.
fibonacci(N, X) :-

    % Call the helper function and store the list.
    fib_builder(N, R),

    % Reverse the list to return the sequence.
    reverse(R, X), !.