#include "parser.h"


#define help_text	"Graphs database interface\n" \
					"Enter 'help' to see this text.\n" \
					"Enter 'exit'/'quit' to quit the program.\n" \
					"\n" \
					"Usage:   'keyword -arg1 -arg2 arg3 arg4 ...' (without quotation marks)\n" \
					"Example: import -g6 resources/graphs.g6\n" \
					"\n" \
					"Arguments in round brackets '()' are optional.\n" \
					"Square brackets '[]' indicate a placeholder for an argument.\n" \
					"Order of arguments without minus '-' in front DOES matter.\n" \
					"A slash '/' between arguments indicates mutual exclusivity.\n" \
					"\n" \
					"Enter 'help ([keyword])' to learn how to use the various features of this program.\n" \
					"\n" \
					"--Example: help insert\n" \
					"\n" \
					"--Valid 'keyword' arguments:\n" \
					"     sql     : shows a sample of useful SQL queries instead\n" \
					"     import  : shows a description of the 'import' functionality\n" \
					"     numbers : shows a description of the 'numbers' functionality\n" \
					"     type    : shows a description of the 'type' functionality\n" \
					"     scripts : shows a description of the 'scripts' functionality\n" \
					"     results : shows a description of the 'results' functionality\n" \
					"     show    : shows a description of the 'show' functionality\n" \
					"     save    : shows a description of the 'save' functionality\n" \
					"\n" \
					"Every input not starting with a keyword will be interpreted as an SQL query. Results from queries will be saved in memory as the current view.\n"


#define sql_text	"Some SQL queries (note that sqlite is not case-sensitive, but this program only accepts 'SELECT', 'Select' and 'select' for queries):\n" \
					"\n" \
					"SELECT * FROM Graphs WHERE graphOrder == 3;\n" \
					"SELECT graphID, graphOrder, graphSize FROM Graphs WHERE graphOrder == 7 AND graphSize == 4;\n" \
					"SELECT DISTINCT graphOrder FROM Graphs WHERE graphSize == 4;\n" \
					"SELECT graphSize FROM Graphs ORDER BY graphSize DESC LIMIT 5;\n" \
					"SELECT MAX(graphSize) FROM Graphs;\n" \
					"SELECT graphOrder, count(*) FROM Graphs WHERE type LIKE \"%cograph%\" GROUP BY graphOrder;\n" \
					"SELECT graphOrder, graphSize, CASE WHEN graphSize <= 1.5 * graphOrder THEN \"sparse\" ELSE \"dense\" AS adjacencies FROM Graphs;\n" \
					"SELECT count(CASE WHEN type LIKE \"%cograph%\" THEN 1 END) AS cographs,\n" \
					"  count(CASE WHEN type LIKE \"%closed%\" THEN 1 END) AS closedGraphs\n" \
					"  FROM Graphs;\n" \
					"SELECT t.type, count(g.graphID) AS count\n" \
					"  FROM (SELECT \"connected\" AS type\n" \
					"    UNION SELECT \"cograph\"\n" \
					"    UNION SELECT \"euler\"\n" \
					"    UNION SELECT \"chordal\"\n" \
					"    UNION SELECT \"claw-free\"\n" \
					"    UNION SELECT \"closed\"\n" \
					"  ) AS t\n" \
					"  INNER JOIN Graphs AS g\n" \
					"  ON g.type LIKE \"%\" || t.type || \"%\"\n" \
					"  GROUP BY t.type ORDER BY count;\n" \
					"\n" \
					"UPDATE Graphs SET type = NULL, cliqueNumber = -1 WHERE graphID <> 1;\n" \
					"DELETE FROM Graphs WHERE NOT type IS NULL;\n"


#define insert_text	"Enter 'import -[format] [file name]' to import all graphs from the file with the specified path (they are expected to be in the specified format).\n" \
					"\n" \
					"--Example: import -list \"resources/graphs.txt\"\n" \
					"\n" \
					"--Valid 'format' arguments:\n" \
					"     -g6   : The 'Graph6' format is an efficient format for storing undirected graphs.\n" \
					"             It is the default format of Nauty, a great tool for easy graph generation.\n" \
					"     -list : A readable, but inefficient format.\n" \
					"             One graph per line, defined by graph order, followed by a space, then the list of edges.\n" \
					"             Example : '3 {1,2},{2,3}'.\n" \
					"     -adj  : A readable, but inefficient format selectable in Nauty.\n" \
					"             Example : (empty line)\n" \
					"                       Graph 1, order 3.\n" \
					"                         1 : 3;\n" \
					"                         2 : 3;\n" \
					"                         3 : 1 2; \n" \
					"\n" \
					"--The 'file name' must specify a relative path to a correctly formatted text file. It can be entered with or without quotation marks '\"'.\n"


#define numbers_text	"Enter 'numbers (-allexcept) (-[numbers1] -[numbers2] ...) (-where [condition])' to compute all specified numbers of the graphs in the database.\n" \
						"\n" \
						"--Example: numbers -clique -where \"graphOrder == 4\"\n" \
						"\n" \
						"--The argument '-allexcept' is used to compute all numbers except the ones specified. It can be used to compute all numbers.\n" \
						"\n" \
						"--Valid number arguments:\n" \
						"     -clique : Computes the clique number and the number of maximal cliques of each graph.\n" \
						"     -detour : Computes the detour number of each graph, i.e., the length of the longest induced path.\n" \
						"     -degree : Computes the minimum and maximum degree of vertices in each graph.\n" \
						"     -stable : Computes the independence number and the number of maximal independent (or stable) sets of each graph.\n" \
						"     -girth  : Computes the girth of each graph, i.e., the minimum length of a cycle (or 0 if there are none).\n" \
						"\n" \
						"--The argument '-where' is used to indicate a following SQL query condition (in quotation marks '\"'). Only graphs satisfying this additional condition will be updated.\n"


#define type_text	"Enter 'type (-allexcept) (-[type1] -[type2] ...) (-where [condition])' to label all graphs of the specified types in the database as such.\n" \
					"\n" \
					"--Example: type -allexcept -chordal -closed -where \"graphOrder < 5\"\n" \
					"\n" \
					"--The argument '-allexcept' is used to label all types except the ones specified. It can be used to label all types.\n" \
					"\n" \
					"--Valid type arguments:\n" \
					"     -connected\n" \
					"     -cograph\n" \
					"     -euler\n" \
					"     -chordal\n" \
					"     -claw-free\n" \
					"     -closed      (This requires the chordal and claw-free graphs to be labeled first.)\n" \
					"     -cone\n" \
					"\n" \
					"--The argument '-where' is used to indicate a following SQL query condition (in quotation marks '\"'). Only graphs satisfying this additional condition will be labeled.\n"


#define scripts_text	"Enter 'scripts [name] -[result type] (-[labeling]) (-[batch size]) ([template name]) (-where [condition])' to generate Macaulay2 scripts based on a template.\n" \
						"\n" \
						"--Example: scripts bei -closed -2500 template.m2 -where \"type LIKE '%closed%'\"\n" \
						"\n" \
						"--The 'name' specifies the names of the columns which will be created for the results of the scripts as well as the file names.\n" \
						"\n" \
						"--Valid result type arguments:\n" \
						"     -betti   : Script results will be expected to be Betti tables generated by Macaulay2.\n" \
						"     -hpoldeg : Script results will be expected to be degrees of h-polynomials (numerator of reduced Hilbert series).\n" \
						"\n" \
						"--Valid labeling arguments:\n" \
						"     -closed : A kind of labeling with regards to which each graph is closed (does not make sense for non-closed graphs).\n" \
						"\n" \
						"--The 'batch size' must be a positive integer (default is 2500). It specifies the number of graphs to be written into each script.\n" \
						"\n" \
						"--The 'template name' must specify a relative path to a template for a Macaulay2 script (default is \"template_[index].m2\").\n" \
						"\n" \
						"--The argument '-where' is used to indicate a following SQL query condition (in quotation marks '\"'). Only graphs satisfying this additional condition will be included in the scripts.\n"


#define results_text	"Enter 'results (-[script id])' to add (Macaulay2-generated) result data to graphs for which Macaulay2 scripts were generated before.\n" \
						"\n" \
						"--Example: results -1\n" \
						"\n" \
						"--The 'script id' must be a valid row id from the internal 'Scripts' table of generated scripts. When omitted, the program will output the 'Scripts' table to the terminal and ask for a valid 'script id'.\n"


#define show_text	"Enter 'show (-[limit]) (-f) (-r)' to output the current view to the terminal.\n" \
					"\n" \
					"--The 'limit' must be a positive integer (default is 100 for normal format and 25 for rich format). The program will output (at most) the specified number of graphs.\n" \
					"\n" \
					"--The argument '-f' forces the output of all graphs in the current view (use with caution).\n" \
					"\n" \
					"--The argument '-r' stands for 'rich output format'. In this format, tables will be displayed as actual tables, not lists.\n"


#define save_text	"Enter 'save (-r/-v/-g6) ([file name])' to ouput the current view to a text file.\n" \
					"\n" \
					"--The argument '-r' stands for 'rich output format'. In this format, tables will be written as actual tables, not lists.\n" \
					"\n" \
					"--The argument '-v' will cause the program to output LaTeX code snippets, using the TikZ package for easy visualization of all graphs in the current view.\n" \
					"\n" \
					"--The argument '-g6' stands for the 'Graph6' format, an efficient format for storing undirected graphs.\n"


#define NUMBER_FORMATS 3
typedef bool (Graph::*Read_next_format)(std::ifstream * file);
const char* FORMATS[NUMBER_FORMATS] = { "g6", "list", "adj" };
Read_next_format READERS[NUMBER_FORMATS] = { &Graph::read_next_g6_format, &Graph::read_next_list_format, &Graph::read_next_adjacency_format };

#define NUMBER_NUMBERVECTORS 5
typedef std::vector<unsigned>(Graph::*Graph_numbers) ();
const char* NUMBERVECTORS[NUMBER_NUMBERVECTORS] = { "clique", "detour", "degree", "stable", "girth" };
std::vector<const char*> COLUMNVECTORS[NUMBER_NUMBERVECTORS] = { { "cliqueNumber", "maxCliqueNumber" }, { "detourNumber" }, { "minDeg", "maxDeg" }, { "stableNumber", "maxStableNumber" }, { "girth" } };
Graph_numbers GETTERS[NUMBER_NUMBERVECTORS] = { &Graph::get_clique_numbers, &Graph::get_detour_number, &Graph::get_extreme_degrees, &Graph::get_independence_numbers, &Graph::get_girth };

#define NUMBER_TYPES 7
typedef bool (Graph::*Graph_test) ();
const char* TYPES[NUMBER_TYPES] = { "connected", "cograph", "euler", "chordal", "claw-free", "closed", "cone" };
const char* PRINT_NAMES[NUMBER_TYPES] = { "connected graphs", "cographs", "euler graphs", "chordal graphs", "claw-free graphs", "closed graphs", "cone graphs" };
Graph_test TESTS[NUMBER_TYPES] = { &Graph::is_connected, &Graph::is_cograph, &Graph::is_euler, &Graph::is_chordal, &Graph::is_clawfree, &Graph::is_closed, &Graph::is_cone };
const char* STD_CONDITIONS[NUMBER_TYPES] = { 0, 0, 0, 0, 0, "type LIKE '%chordal%' AND type LIKE '%claw-free%'", 0 };

#define NUMBER_LABELINGS 1
typedef unsigned* (Graph::*Gen_labeling) ();
const char* LABELINGS[NUMBER_LABELINGS] = { "closed" };
Gen_labeling GENERATORS[NUMBER_LABELINGS] = { &Graph::gen_closed_labeling };

#define NUMBER_RESULTS 2
typedef bool (DatabaseInterface::*Result_inserter) (std::string * ideal, std::string * query_condition, std::string * datetime, unsigned index);
const char* RESULTS[NUMBER_RESULTS] = { "betti", "hpoldeg" };
Result_inserter INSERTERS[NUMBER_RESULTS] = { &DatabaseInterface::insert_betti_data, &DatabaseInterface::insert_hpoldeg_data };


//########## helper functions ##########
std::string cut_first_argument(std::string * input) {
	while (!input->empty() && input->front() == ' ')
		*input = input->substr(1, std::string::npos);

	if (input->empty())
		return "";

	size_t cut_index;
	std::string arg;
	
	if (input->front() == '"')
	{
		arg = input->substr(1, std::string::npos);
		cut_index = arg.find_first_of('"');

		if (cut_index == std::string::npos)
		{
			PARSE_ERROR("Closing '\"' missing.");
			return "";
		}

		arg = arg.substr(0, cut_index);
		cut_index += 2;
	}
	else
	{
		cut_index = input->find_first_of(' ');
		arg = input->substr(0, cut_index);
	}

	if (cut_index < input->length() - 1)
		*input = input->substr(cut_index + 1, std::string::npos);
	else
		*input = "";

	return arg;
}


int parse_unsigned(std::string * arg) {
	int number = 0;

	for (unsigned i = 0; i < arg->length(); i++)
	{
		if (arg->at(i) < '0'
			|| arg->at(i) > '9')
		{
			PARSE_ERROR("'" << *arg << "' is not a number.");
			return -1;
		}

		number *= 10;
		number += arg->at(i) - '0';
	}

	return number;
}


//########## parse functions ##########
/**
* outputs help texts to the terminal
**/
void help_parse(DatabaseInterface * dbi, std::string * input) {
	bool sql = false;
	bool insert = false;
	bool numbers = false;
	bool label = false;
	bool scripts = false;
	bool result = false;
	bool show = false;
	bool save = false;

	int argc = 0;

	while (!input->empty())
	{
		std::string arg = cut_first_argument(input);
		if (arg.empty())
		{
			if (input->empty())
				break;
			return;
		}
		argc++;

		if (arg == "sql")
			sql = true;
		else if (arg == "insert")
			insert = true;
		else if (arg == "numbers")
			numbers = true;
		else if (arg == "label")
			label = true;
		else if (arg == "scripts")
			scripts = true;
		else if (arg == "results")
			result = true;
		else if (arg == "show")
			show = true;
		else if (arg == "save")
			save = true;
		else{
			INVALID_ARG();
			return;
		}
	}

	if (argc == 0)
		std::cout << help_text << std::endl;
	else if (argc == 1)
	{
		if (sql)
			std::cout << sql_text << std::endl;
		else if (insert)
			std::cout << insert_text << std::endl;
		else if (numbers)
			std::cout << numbers_text << std::endl;
		else if (label)
			std::cout << type_text << std::endl;
		else if (scripts)
			std::cout << scripts_text << std::endl;
		else if (result)
			std::cout << results_text << std::endl;
		else if (show)
			std::cout << show_text << std::endl;
		else if (save)
			std::cout << save_text << std::endl;
	}
	else
		TOO_MANY_ARG();
}


/**
* parses the arguments for import
**/
void import_parse(DatabaseInterface * dbi, std::string * input) {
	int format = -1;
	std::string filename = "";

	while (!input->empty())
	{
		bool match = false;
		std::string arg = cut_first_argument(input);
		if (arg.empty())
		{
			if (input->empty())
				break;
			FAIL("Importing graphs", "");
			return;
		}

		for (int i = 0; i < NUMBER_FORMATS; i++)
		{
			if (arg == "-" + std::string(FORMATS[i]))
			{
				if (format != -1)
				{
					TOO_MANY_ARG();
					FAIL("Importing graphs", "Format already specified.");
					return;
				}
				format = i;
				match = true;
				break;
			}
		}

		if (!match && filename.empty())
		{
			filename = arg;
			match = true;
		}

		if (!match)
		{
				EITHER_ARG();
				FAIL("Importing graphs", "");
				return;
		}
	}

	if (format == -1)
	{
		FAIL("Importing graphs", "No format specified.");
		return;
	}

	std::ifstream file(filename);
	if (!file.is_open())
	{
		FAIL("Importing graphs", "Unable to open '" << filename << "'.");
		return;
	}

	PROGRESS(3, "importing graphs");
	dbi->import_graphs(&file, READERS[format]);
	file.close();
}


/**
* parses the arguments for numbers
**/
void numbers_parse(DatabaseInterface * dbi, std::string * input) {
	bool allexcept = false;
	std::vector<bool> to_be_computed;
	bool condition = false;
	std::string query_condition = "";

	for (int i = 0; i < NUMBER_NUMBERVECTORS; i++)
		to_be_computed.push_back(false);

	while (!input->empty())
	{
		bool match = false;
		std::string arg = cut_first_argument(input);
		if (arg.empty())
		{
			if (input->empty())
				break;
			FAIL("Computing numbers", "");
			return;
		}

		if (condition)
		{
			if (query_condition.empty())
				query_condition = arg;
			else
				query_condition += " AND " + arg;
			condition = false;
		}
		else
		{
			if (arg == "-allexcept")
			{
				allexcept = true;
				match = true;
			}
			else if (arg == "-where")
			{
				condition = true;
				match = true;
			}
			else
			{
				for (int i = 0; i < NUMBER_NUMBERVECTORS; i++)
				{
					if (arg == "-" + std::string(NUMBERVECTORS[i]))
					{
						to_be_computed[i] = true;
						match = true;
						break;
					}
				}
			}

			if (!match)
			{
				INVALID_ARG();
				FAIL("Computing numbers", "");
				return;
			}
		}
	}

	if (allexcept)
	{
		for (int i = 0; i < NUMBER_NUMBERVECTORS; i++)
			to_be_computed[i] = !to_be_computed[i];
	}

	for (int i = 0; i < NUMBER_NUMBERVECTORS; i++)
	{
		if (to_be_computed[i])
		{
			PROGRESS(1, "computing " << NUMBERVECTORS[i] << " numbers");
			dbi->update_numbers(GETTERS[i], &(COLUMNVECTORS[i]), query_condition.empty() ? 0 : query_condition.c_str());
		}
	}
}


/**
* parses the arguments for type
**/
void type_parse(DatabaseInterface * dbi, std::string * input) {
	bool allexcept = false;
	std::vector<bool> to_be_labeled;
	bool condition = false;
	std::string query_condition = "";

	for (int i = 0; i < NUMBER_TYPES; i++)
		to_be_labeled.push_back(false);

	while (!input->empty())
	{
		bool match = false;
		std::string arg = cut_first_argument(input);
		if (arg.empty())
		{
			if (input->empty())
				break;
			FAIL("Labeling graphs", "");
			return;
		}

		if (condition)
		{
			if (query_condition.empty())
				query_condition = arg;
			else
				query_condition += " AND " + arg;
			condition = false;
		}
		else
		{
			if (arg == "-allexcept")
			{
				allexcept = true;
				match = true;
			}
			else if (arg == "-where")
			{
				condition = true;
				match = true;
			}
			else
			{
				for (int i = 0; i < NUMBER_TYPES; i++)
				{
					if (arg == "-" + std::string(TYPES[i]))
					{
						to_be_labeled[i] = true;
						match = true;
						break;
					}
				}
			}

			if (!match)
			{
				INVALID_ARG();
				FAIL("Labeling graphs", "");
				return;
			}
		}
	}

	if (allexcept)
	{
		for (int i = 0; i < NUMBER_TYPES; i++)
			to_be_labeled[i] = !to_be_labeled[i];
	}

	for (int i = 0; i < NUMBER_TYPES; i++)
	{
		if (to_be_labeled[i])
		{
			PROGRESS(1, "labeling " << PRINT_NAMES[i]);

			if (STD_CONDITIONS[i])
				dbi->update_type(TESTS[i], TYPES[i], query_condition.empty() ? STD_CONDITIONS[i] : (std::string(STD_CONDITIONS[i]) + " AND " + query_condition).c_str());
			else
				dbi->update_type(TESTS[i], TYPES[i], query_condition.empty() ? 0 : query_condition.c_str());
		}
	}
}


/**
* parses the arguments for gen_M2_scripts and calls gen_m2_scripts with the appropriate arguments
**/
void script_parse(DatabaseInterface * dbi, std::string * input) {
	bool condition = false;
	int batch_size = -1;
	int labeling = -1;
	int result = -1;
	std::string name = "";
	std::string filename = "";
	std::string query_condition = "";

	while (!input->empty())
	{
		bool match = false;
		std::string arg = cut_first_argument(input);
		if (arg.empty())
		{
			if (input->empty())
				break;
			FAIL("Generating M2 scripts", "");
			return;
		}

		if (condition)
		{
			if (query_condition.empty())
				query_condition = arg;
			else
				query_condition += " AND " + arg;
			condition = false;
		}
		else
		{
			if (arg == "-where")
			{
				condition = true;
				match = true;
			}
			else if (arg.front() == '-')
			{
				for (int i = 0; i < NUMBER_LABELINGS; i++)
				{
					if (arg == "-" + std::string(LABELINGS[i]))
					{
						if (labeling != -1)
						{
							TOO_MANY_ARG();
							FAIL("Generating M2 scripts", "Labeling already specified.");
							return;
						}
						labeling = i;
						match = true;
						break;
					}
				}

				if (!match)
				{
					for (int i = 0; i < NUMBER_RESULTS; i++)
					{
						if (arg == "-" + std::string(RESULTS[i]))
						{
							if (result != -1)
							{
								TOO_MANY_ARG();
								FAIL("Generating M2 scripts", "Result type already specified.");
								return;
							}
							result = i;
							match = true;
							break;
						}
					}
				}

				if (!match && batch_size == -1)
				{
					arg = arg.substr(1, std::string::npos);
					batch_size = parse_unsigned(&arg);
					if (batch_size == -1)
					{
						arg = "-" + arg;
						INVALID_ARG();
						FAIL("Generating M2 scripts", "");
						return;
					}
					match = true;
				}
			}
			else if (name.empty())
			{
				name = arg;
				match = true;
			}
			else if (filename.empty())
			{
				filename = arg;
				match = true;
			}

			if (!match)
			{
				EITHER_ARG();
				FAIL("Generating M2 scripts", "");
				return;
			}
		}
	}

	if (name.empty())
	{
		FAIL("Generating M2 scripts", "No ideal name specified.");
		return;
	}

	if (result == -1)
	{
		FAIL("Generating M2 scripts", "No result type specified.");
		return;
	}

	if (name.find("Bettis") == std::string::npos)
		dbi->generate_m2_scripts(&name, labeling == -1 ? 0 : GENERATORS[labeling], batch_size == -1 ? 2500 : batch_size, query_condition.empty() ? 0 : query_condition.c_str(), filename.empty() ? 0 : filename.c_str(), labeling == -1 ? 0 : LABELINGS[labeling], result);
	else
		FAIL("Generating M2 scripts", "Please choose a name other than '" << name << "'. Any name containing 'Bettis' would interfere with the program.");
}


/**
* parses the arguments for result
**/
void results_parse(DatabaseInterface * dbi, std::string * input) {
	int scriptID = -1;

	while (!input->empty())
	{
		std::string arg = cut_first_argument(input);
		if (arg.empty())
		{
			if (input->empty())
				break;
			FAIL("Adding result data", "");
			return;
		}

		if (arg.front() != '-')
		{
			INVALID_ARG();
			FAIL("Adding result data", "");
			return;
		}

		if (scriptID == -1)
		{
			arg = arg.substr(1, std::string::npos);
			scriptID = parse_unsigned(&arg);
			if (scriptID == -1)
			{
				arg = "-" + arg;
				INVALID_ARG();
				FAIL("Adding result data", "");
				return;
			}
		}
		else
		{
			TOO_MANY_ARG();
			FAIL("Adding result data", "");
			return;
		}
	}

	if (scriptID == -1)
	{
		dbi->show_scripts();

		while (scriptID == -1)
		{
			INPUT("Please select a scriptID to add the results of the respective M2 scripts.");

			std::string arg;
			if (!getline(std::cin, arg))
				return;

			SEPARATE();

			scriptID = parse_unsigned(&arg);
		}
	}

	std::string name, query_condition, datetime;
	int result = dbi->find_script_data(scriptID, &name, &query_condition, &datetime);

	if (result == -1)
		return;

	PROGRESS(1, "adding result data");
	if ((dbi->*INSERTERS[result])(&name, &query_condition, &datetime, result))
		dbi->execute_SQL_statement(&("DELETE FROM Scripts WHERE scriptID == " + std::to_string(scriptID)));
}


/**
* parses the arguments for show to decide the output format and the limit for the number of output rows, then chooses the appropriate function
**/
void show_parse(DatabaseInterface * dbi, std::string * input) {
	int limit = -1;
	bool force = false;
	bool rich = false;
	bool format = false;

	while (!input->empty())
	{
		std::string arg = cut_first_argument(input);
		if (arg.empty())
		{
			if (input->empty())
				break;
			return;
		}

		if (arg == "-r")
		{
			if (format)
			{
				TOO_MANY_ARG();
				return;
			}
			rich = true;
			format = true;
		}
		else if (arg == "-f")
		{
			if (limit != -1)
				WARNING("'-f' will be ignored. A limit was specified.");
			else
				force = true;
		}
		else if (arg.front() == '-')
		{
			if (limit != -1)
			{
				EITHER_ARG();
				return;
			}
			if (force)
			{
				WARNING("'-f' will be ignored. A limit was specified.");
				force = false;
			}
			else
			{
				arg = arg.substr(1, std::string::npos);
				limit = parse_unsigned(&arg);

				if (limit == -1)
				{
					arg = "-" + arg;
					INVALID_ARG();
					return;
				}
			}
		}
		else
		{
			INVALID_ARG();
			return;
		}
	}

	if (force)
	{
		if (rich)
			dbi->show_view_rich(-1);
		else
			dbi->show_view(-1);
	}
	else
	{
		if (limit == -1)
		{
			if (rich)
				dbi->show_view_rich();
			else
				dbi->show_view();
		}
		else
		{
			if (rich)
				dbi->show_view_rich(limit);
			else
				dbi->show_view(limit);
		}
	}
}


/**
* parses the arguments for save to decide the output format and the file name, tries to open the file, then chooses the appropriate function
**/
void save_parse(DatabaseInterface * dbi, std::string * input) {
	std::string filename = "";
	bool rich = false;
	bool visualisation = false;
	bool g6 = false;
	bool format = false;

	while (!input->empty())
	{
		std::string arg = cut_first_argument(input);
		if (arg.empty())
		{
			if (input->empty())
				break;
			FAIL("Saving view", "");
			return;
		}

		if (arg == "-r")
		{
			if (format)
			{
				TOO_MANY_ARG();
				FAIL("Saving view", "Format already specified.");
				return;
			}
			rich = true;
			format = true;
		}
		else if (arg == "-v")
		{
			if (format)
			{
				TOO_MANY_ARG();
				FAIL("Saving view", "Format already specified.");
				return;
			}
			visualisation = true;
			format = true;
		}
		else if (arg == "-g6")
		{
			if (format)
			{
				TOO_MANY_ARG();
				FAIL("Saving view", "Format already specified.");
				return;
			}
			g6 = true;
			format = true;
		}
		else if (filename.empty())
		{
			filename = arg + ".view";
			std::ifstream test(filename);

			if (test.is_open())
			{
				WARNING("'" << filename << "' already exists.");
				INPUT("Override? (y/n)");

				std::string extra_input;
				while (true)
				{
					getline(std::cin, extra_input);
					if (extra_input == "y")
						break;
					if (extra_input == "n")
					{
						test.close();
						return;
					}
					PARSE_ERROR("Not a valid input. Try again.");
					INPUT("");
				}
			}

			test.close();
		}
		else
		{
			EITHER_ARG();
			FAIL("Saving view", "");
			return;
		}
	}

	if (filename.empty())
		filename = "database.view";

	std::ofstream file(filename, std::ios::trunc);

	if (!file.is_open())
	{
		FAIL("Saving view", "Unable to open '" << filename << "'.");
		return;
	}

	if (rich)
		dbi->save_view_rich(&file);
	else if (visualisation)
		dbi->save_view_visualisation(&file);
	else if (g6)
		dbi->save_view_g6(&file);
	else
		dbi->save_view(&file);

	file.close();

	if (visualisation
		|| g6)
		RESULT("Saved graphs in current view to '" << filename << "'.");
	else
		RESULT("Saved current view to '" << filename << "'.");
}
