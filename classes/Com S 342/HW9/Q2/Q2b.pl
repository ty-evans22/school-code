% Define the function to sort a list of numbers in ascending order.
sortList(List, Sorted) :-

    % Call insertion sort on the list.
    insertion_sort(List, [], Sorted), !.

% Define the base case for insertion sort when the list is empty.
insertion_sort([], A, A).

% Define the case for insertion sort when the list is not empty.
insertion_sort([H|T], A, Sorted) :-

    % Call the insert function on the first element in the list.
    insert(H, A, NA),

    % Recursively call insertion sort to continue sorting the list.
    insertion_sort(T, NA, Sorted).
   
% Define the insert function for the case where the current value is > the next value.
insert(X, [Y|T], [Y|NT]) :-

    % Check if the current value is > the next value.
    X > Y,

    % Recursively call the insert function on the value after the next value.
    insert(X, T, NT).

% Define the insert function for the case where the current value is <= the next value.
insert(X, [Y|T], [X,Y|T]) :-

    % Check if the current value is <= the next value.
    X =< Y.

% Define the base case for the insert function where there is no next value.
insert(X, [], [X]).