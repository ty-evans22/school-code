% Define the finish function to make sure Y exists after H.
finish([H|T], X, Y) :-
    X == H,
    member(Y, T).

finish([_H|T], X, Y) :-
    finish(T, X, Y).

% Define the function to solve the puzzle. Run by executing the query "finish_order(X)."
finish_order(Order) :-
    permutation([a, b, c, d, e], Order),
    forall(
        member(X < Y, [a < b, c < a, d < e, b < d]),
        finish(Order, X, Y)
    ), !.