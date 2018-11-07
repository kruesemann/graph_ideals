-- Macaulay2 template for script generation
-- Scripts will compute if the graphs G_i are Koszul, meaning the maximal ideal of the quotient ring of the binomial edge ideal of G_i has a linear resolution.
-- DO:      Import all required packages, change "bei" (in loop) to desired graph ideal.
-- DO NOT:  Define values of "G", "N" and "filename" as they will be defined by the automated script generation.

needsPackage("BinomialEdgeIdeals");
needsPackage("QuillenSuslin");

for i from 0 to N-1 do {
J = bei G_i
I = getMaxIdeal promote(ideal(), ring J / J);
K_i = regularity res I == 0;
};

F:=openOut(filename);
for i from 0 to N-1 do F << K_i;
close F;