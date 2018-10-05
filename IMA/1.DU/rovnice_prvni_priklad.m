syms A B C D E
eqn1 = A + C + D == 0;
eqn2 = 4*A + B + 3*C + 5*D + E == -1;
eqn3 = 5*A + B + 3*C + 8*D + 5*E == -6;
eqn4 = 4*A + B +3*C + 4*D + 8*E == 1;
eqn5 = 4*A + B + 2*C + 4*E == 8;

[X, Y] = equationsToMatrix([eqn1, eqn2, eqn3, eqn4, eqn5], [A, B, C, D, E])
Z = linsolve(X, Y) 