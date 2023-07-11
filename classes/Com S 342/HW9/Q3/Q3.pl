% Define the words from the poem.
word(i).
word(m,y).
word(o,n).
word(a,n,d).
word(o,u,t).
word(t,h,e).
word(e,v,e,r).
word(o,p,e,n).
word(d,o,o,r).
word(l,o,o,k).
word(a,g,a,i,n).
word(f,r,i,e,n,d).
word(d,a,r,k,n,e,s,s).

% Define the function to solve the word puzzle.
puzzle(X, Y) :-
    word(A, B, C, D, E, F, G, H),
    word(I, J, F, K),
    atomic_list_concat([A, B, C, D, E, F, G, H], X),
    atomic_list_concat([I, J, F, K], Y), !.