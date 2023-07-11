% Define the neighbor function for the case when the list is NOT empty.
neighbor(V, [H|T], X) :-
    search([], V, [H|T], X), !.

% Define the search function for the base case when there are no more values to be searched.
search(_P, _V, [], []).

% Define the search function for the case when there are still values to be searched.
search(P, V, [H|T], X) :-

    % If-Then-Else definition for the search function.
    (
        % Check if the head == the correct value.
        H == V
        
        % Case when the head == the correct value.
        -> X = P

        % Case when the head != the correct value.
        ; search(H, V, T, X)
    ).
