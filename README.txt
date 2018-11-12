Tinait - Graphs database interface

A console application to build and access databases for graphs detailing both their own properties
 and those of associated polynomial ideals.
Uses sqlite3 for database interaction (available at https://www.sqlite.org/).

Tinait stands for 'Tinait is not an ideal tool'.

For a step-by-step guide to building a complete database, see STEP-BY-STEP.txt.
For a guide to add new values and properties to the database or import formats and orderings to the application, see EXTENDING_FUNCTIONALITIES.txt.

Contents:
1. Help
2. Importing graphs
3. Computing graph-theoretic values
4. Classifying graphs
5. Generating Macaulay2 scripts for computation of algebraic values of associated ideals
6. Importing result data from Macaulay2 scripts
7. Printing current database view to the console
8. Printing current database view to a text file
9. Sample of SQL statements


=========================================================================================
1. HELP
=========================================================================================

Enter 'help' to see this text.
Enter 'exit'/'quit' to quit the program.

Usage:   'keyword -arg1 -arg2 arg3 arg4 ...' (without quotation marks)
Example: import -g6 resources/graphs.g6

Arguments in round brackets '()' are optional.
Square brackets '[]' indicate a placeholder for an argument.
Order of arguments without minus '-' in front DOES matter.
A slash '/' between arguments indicates mutual exclusivity.

Enter 'help ([keyword])' to learn how to use the various features of this program.

--Example: help insert

--Valid 'keyword' arguments:
     sql      : shows a sample of useful SQL queries instead
     import   : shows a description of the 'import' functionality
     compute  : shows a description of the 'compute' functionality
     classify : shows a description of the 'classify' functionality
     scripts  : shows a description of the 'scripts' functionality
     results  : shows a description of the 'results' functionality
     show     : shows a description of the 'show' functionality
     save     : shows a description of the 'save' functionality

Every input not starting with a keyword will be interpreted as an SQL query. Results from queries will be saved in memory as the current view.

=========================================================================================
2. IMPORTING GRAPHS
=========================================================================================

Enter 'import -[format] [file name]' to import all graphs from the file with the specified path (they are expected to be in the specified format).

--Example: import -list "resources/graphs.txt"

--Valid 'format' arguments:
     -g6   : The 'Graph6' format is an efficient format for storing undirected graphs.
             It is the default format of Nauty, a great tool for easy graph generation.
     -list : A readable, but inefficient format.
             One graph per line, defined by graph order, followed by a space, then the list of edges.
             Example : '3 {1,2},{2,3}'.

--The 'file name' must specify a relative path to a correctly formatted text file. It can be entered with or without quotation marks '"'.

=========================================================================================
3. COMPUTING GRAPH-THEORETIC VALUES
=========================================================================================

Enter 'compute (-allexcept) (-[valueSet1] -[valueSet2] ...) (-where [condition])' to compute all specified value sets of the graphs in the database.

--Example: compute -clique -where "graphOrder == 4"

--The argument '-allexcept' is used to compute all value sets except the ones specified. It can be used to compute all value sets.

--Valid value set arguments:
     -clique       : Computes the clique number and the number of maximal cliques of each graph.
     -detour       : Computes the detour number of each graph, i.e., the length of the longest induced path.
     -degree       : Computes the minimum and maximum degree of vertices in each graph.
     -independence : Computes the independence number and the number of maximal independent (or stable) sets of each graph.
     -girth        : Computes the girth of each graph, i.e., the minimum length of a cycle (or 0 if there are none).

--The argument '-where' is used to indicate a following SQL query condition (in quotation marks '"'). Only graphs satisfying this additional condition will be updated.

=========================================================================================
4. CLASSIFYING GRAPHS
=========================================================================================

Enter 'classify (-allexcept) (-[type1] -[type2] ...) (-where [condition])' to classify all graphs of the specified types in the database as such.

--Example: classify -allexcept -chordal -closed -where "graphOrder < 5"

--The argument '-allexcept' is used to classify all types except the ones specified. It can be used to classify all types.

--Valid type arguments:
     -connected
     -cograph
     -euler
     -chordal
     -claw-free
     -closed      (This requires the chordal and claw-free graphs to be classified first.)
     -cone

--The argument '-where' is used to indicate a following SQL query condition (in quotation marks '"'). Only graphs satisfying this additional condition will be classified.

=========================================================================================
5. GENERATING MACAULAY2 SCRIPTS FOR COMPUTATION OF ALGEBRAIC VALUES OF ASSOCIATED IDEALS
=========================================================================================

Enter 'scripts [name] -[result type] (-[ordering]) (-[batch size]) ([template name]) (-where [condition])' to generate Macaulay2 scripts based on a template.

--Example: scripts bei -closed -2500 template.m2 -where "type LIKE '%closed%'"

--The 'name' specifies the names of the columns which will be created for the results of the scripts as well as the file names.

--Valid result type arguments:
     -betti   : Script results will be expected to be Betti tables generated by Macaulay2.
     -hpoldeg : Script results will be expected to be degrees of h-polynomials (numerator of reduced Hilbert series).

--Valid ordering arguments:
     -closed : An ordering inducing a labeling with respect to which each graph is closed (does not make sense for non-closed graphs).

--The 'batch size' must be a positive integer (default is 2500). It specifies the number of graphs to be written into each script.

--The 'template name' must specify a relative path to a template for a Macaulay2 script (default is "template_[index].m2").

--The argument '-where' is used to indicate a following SQL query condition (in quotation marks '"'). Only graphs satisfying this additional condition will be included in the scripts.

=========================================================================================
6. IMPORTING RESULT DATA FROM MACAULAY2 SCRIPTS
=========================================================================================

Enter 'results (-[script id])' to add (Macaulay2-generated) result data to graphs for which Macaulay2 scripts were generated before.

--Example: results -1

--The 'script id' must be a valid row id from the internal 'Scripts' table of generated scripts. When omitted, the program will output the 'Scripts' table to the terminal and ask for a valid 'script id'.

=========================================================================================
7. PRINTING CURRENT DATABASE VIEW TO THE CONSOLE
=========================================================================================

Enter 'show (-[limit]) (-f) (-r)' to output the current view to the terminal.

--The 'limit' must be a positive integer (default is 100 for normal format and 25 for rich format). The program will output (at most) the specified number of graphs.

--The argument '-f' forces the output of all graphs in the current view (use with caution).

--The argument '-r' stands for 'rich output format'. In this format, tables will be displayed as actual tables, not lists.

=========================================================================================
8. PRINTING CURRENT DATABASE VIEW TO A TEXT FILE
=========================================================================================

Enter 'save (-r/-v/-g6) ([file name])' to ouput the current view to a text file.

--The argument '-r' stands for 'rich output format'. In this format, tables will be written as actual tables, not lists.

--The argument '-v' will cause the program to output LaTeX code snippets, using the TikZ package for easy visualization of all graphs in the current view.

--The argument '-g6' stands for the 'Graph6' format, an efficient format for storing undirected graphs.

=========================================================================================
9. SAMPLE OF SQL STATEMENTS
=========================================================================================

Some SQL queries (note that sqlite is not case-sensitive, but this program only accepts 'SELECT', 'Select' and 'select' for queries):

SELECT * FROM Graphs WHERE graphOrder == 3;
SELECT graphID, graphOrder, graphSize FROM Graphs WHERE graphOrder == 7 AND graphSize == 4;
SELECT DISTINCT graphOrder FROM Graphs WHERE graphSize == 4;
SELECT graphSize FROM Graphs ORDER BY graphSize DESC LIMIT 5;
SELECT MAX(graphSize) FROM Graphs;
SELECT graphOrder, count(*) FROM Graphs WHERE type LIKE "%cograph%" GROUP BY graphOrder;
SELECT graphOrder, graphSize, CASE WHEN graphSize <= 1.5 * graphOrder THEN "sparse" ELSE "dense" AS adjacencies FROM Graphs;
SELECT count(CASE WHEN type LIKE "%cograph%" THEN 1 END) AS cographs,
  count(CASE WHEN type LIKE "%closed%" THEN 1 END) AS closedGraphs
  FROM Graphs;
SELECT t.type, count(g.graphID) AS count
  FROM (SELECT "connected" AS type
    UNION SELECT "cograph"
    UNION SELECT "euler"
    UNION SELECT "chordal"
    UNION SELECT "claw-free"
    UNION SELECT "closed"
  ) AS t
  INNER JOIN Graphs AS g
  ON g.type LIKE "%" || t.type || "%"
  GROUP BY t.type ORDER BY count;

UPDATE Graphs SET type = NULL, cliqueNumber = -1 WHERE graphID <> 1;
DELETE FROM Graphs WHERE NOT type IS NULL;
