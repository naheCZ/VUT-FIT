clen([], false).
clen([H|_], H).
clen([_|T], X):-clen(T, X).

prevod([], []).
prevod([H|T], T2):-clen(T, H), prevod(T, T2).
prevod([H|T], [H|T2]):-prevod(T, T2).

maximum([], []).
maximum([H], H).
maximum([H1, H2|T], X):-H1 >= H2,  maximum([H1|T], X).
maximum([H1, H2|T], X):-H1 < H2, maximum([H2|T], X).

indmax([], false).
indmax([_], 1).
indmax([H|T], X):-maximum([H|T], X1), H = X1, X is 1.
indmax([_|T], X):-indmax(T, X1), X is 1 + X1.

minimum([], false).
minimum([H], H).
minimum([H1, H2|T], X):-H1 =< H2, minimum([H1|T], X).
minimum([H1, H2|T], X):-H1 > H2, minimum([H2|T], X).

indmin([], false).
indmin([_], 1).
indmin([H|T], X):-minimum([H|T], X1), H = X1, X is 1.
indmin([_|T], X):-indmin(T, X1), X is X1 + 1.

prunik([], _, []).
prunik([H|T], S2, [H|T2]):-clen(S2, H), prunik(T, S2, T2).
prunik([_|T], S2, X):-prunik(T, S2, X).

podmnozina3(S1, S2, S3):-prunik(S1, S2, P), podmnozina(P, S3).
podmnozina3(_, _, _):-false.

podmnozina([], _):-true.
podmnozina([H|T], S2):-clen(S2, H), podmnozina(T, S2).
podmnozina(_, _):-false.

prvnikladne([H|_], X):-H >= 0, X is H.
prvnikladne([_|T], X):-prvnikladne(T, X).

poslednizaporne([], _).
poslednizaporne([H|T], X):-H < 0, poslednizaporne(T, X).
poslednizaporne([_|T], X):-poslednizaporne(T, X).

reverse([], S, S).
reverse([H|T],S2,S3):-reverse(T, S2, [H|S3]).
