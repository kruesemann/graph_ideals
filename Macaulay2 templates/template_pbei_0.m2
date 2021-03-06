-- Macaulay2 template for script generation
-- Scripts will compute the Betti tables of quotient rings of the parity binomial edge ideals of the graphs G_i.
-- DO:      Import all required packages, change "pbei" (in loop) to desired graph ideal.
-- DO NOT:  Define values of "G", "N" and "filename" as they will be defined by the automated script generation.

needsPackage("BinomialEdgeIdeals");

for i from 0 to N-1 do B_i=betti res pbei G_i;   -- change "pbei" here

F:=openOut(filename);
for i from 0 to N-1 do F << B_i << endl << endl;
close F;