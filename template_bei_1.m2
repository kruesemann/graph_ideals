-- Macaulay2 template for script generation
-- Scripts will compute the degrees of h-polynomials of quotient rings of the binomial edge ideals of the graphs G_i.
-- DO:      Import all required packages, change "bei" (in loop) to desired graph ideal.
-- DO NOT:  Define values of "G", "N" and "filename" as they will be defined by the automated script generation.

needsPackage("BinomialEdgeIdeals");

for i from 0 to N-1 do {
I = bei G_i;                                                     -- change "bei" here
if I === ideal() then I = promote(I, QQ[x_1, x_2]);              -- this is necessary as ideal() defaults to an ideal of ZZ and hilbertSeries does not like that
d_i = first degree numerator hilbertSeries(I, Reduce => true);
};

F:=openOut(filename);
for i from 0 to N-1 do F << d_i << endl;
close F;