% Define the base case when the first and second numbers are equal.
gcd(F, S, X) :-

    % Check if the first number = the second number.
    F = S,

    % Return the first number.
    X = F, !.

% Define the case when the first number < second number.
gcd(F, S, X) :-

    % Check if the first number < the second number.
    F < S,

    % Compute the difference between the first and second numbers.
    S1 is S - F,

    % Recursively call the gcd function with the difference as the new
    % value for the second number.
    gcd(F, S1, X), !.

% Define the case when the first number > second number.
gcd(F, S, X) :-

    % Check if the first number > the second number.
    F > S,

    % Recursively call the gcd function with the first and second numbers swapped.
    gcd(S, F, X), !.