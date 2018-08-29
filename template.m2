-- Macaulay2 template for script generation
-- import all required packages, change "bei" (in loop) to desired graph ideal
-- do not define values of "N" and "filename" as they will be defined by the automated script generation

needsPackage("BinomialEdgeIdeals")
needsPackage("Graphs")

for i from 0 to N do B_i=betti res bei(graph(G_i))

F:=openOut(filename)
for i from 0 to N do F << B_i << endl << endl
close F