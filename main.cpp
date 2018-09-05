#include <iostream>

#include "stdafx.h"
#include "DatabaseInterface.h"


#define help_text	"Graphs database interface\n" \
					"Enter 'help' to see this text.\n" \
					"Enter 'exit'/'quit' to quit the program.\n" \
					"\n" \
					"Usage:   'keyword -arg1 -arg2 arg3 arg4 ...' (without quotation marks)\n" \
					"Example: insert -g6 resources/graphs.g6\n" \
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
					"     insert  : shows a description of the 'insert' functionality\n" \
					"     numbers : shows a description of the 'numbers' functionality\n" \
					"     label   : shows a description of the 'label' functionality\n" \
					"     scripts : shows a description of the 'scripts' functionality\n" \
					"     result  : shows a description of the 'result' functionality\n" \
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


#define insert_text	"Enter 'insert -[format] [file name]' to insert all graphs from the file with the specified path (they are expected to be in the specified format).\n" \
					"\n" \
					"--Example: insert -list \"resources/graphs.txt\"\n" \
					"\n" \
					"--Valid 'format' arguments:\n" \
					"     -g6   : The 'Graph6' format is an efficient format for storing undirected graphs.\n" \
					"             It is the default format of Nauty, a great tool for easy graph generation.\n" \
					"     -list : A readable, but inefficient format.\n" \
					"             One graph per line, defined by graph order, followed by a space, then the list of edges.\n" \
					"             Example : '3 {1,2},{2,3}'.\n" \
					"\n" \
					"--The 'file name' must specify a relative path to a correctly formatted text file. It can be entered with or without quotation marks '\"'.\n"


#define numbers_text	"Enter 'numbers (-allexcept) (-[numbers1] -[numbers2] ...) (-where \"[condition]\")' to compute all specified numbers of the graphs in the database.\n" \
						"\n" \
						"--Example: numbers -clique -where \"graphOrder == 4\"\n" \
						"\n" \
						"--The argument '-allexcept' is used to compute all numbers except the ones specified. It can be used to compute all numbers.\n" \
						"\n" \
						"--Valid number arguments:\n" \
						"     -clique : Computes the clique number and the number of maximal cliques of each graph.\n" \
						"\n" \
						"--The argument '-where' is used to indicate a following SQL query condition (in quotation marks '\"'). Only graphs satisfying this additional condition will be updated.\n"


#define label_text	"Enter 'label (-allexcept) (-[type1] -[type2] ...) (-where \"[condition]\")' to label all graphs of the specified types in the database as such.\n" \
					"\n" \
					"--Example: label -allexcept -chordal -closed -where \"graphOrder < 5\"\n" \
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
					"\n" \
					"--The argument '-where' is used to indicate a following SQL query condition (in quotation marks '\"'). Only graphs satisfying this additional condition will be labeled.\n"


#define scripts_text	"Enter 'scripts [ideal name] -[result type] (-[labeling]) (-[batch size]) ([template name]) (-where \"[condition]\")' to generate Macaulay2 scripts based on a template.\n" \
						"\n" \
						"--Example: scripts bei -closed -2500 template.m2 -where \"type LIKE '%closed%'\"\n" \
						"\n" \
						"--The 'ideal name' specifies the names of the columns which will be created for the results of the scripts as well as the file names.\n" \
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


#define result_text	"Enter 'result (-[script id])' to add (Macaulay2-generated) result data to graphs for which Macaulay2 scripts were generated before.\n" \
					"\n" \
					"--Example: result -1\n" \
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


typedef std::vector<unsigned> (Graph::*Graph_numbers) ();
typedef bool (Graph::*Graph_test) ();
typedef unsigned* (Graph::*Gen_labeling) ();
typedef bool (DatabaseInterface::*Result_inserter) (std::string * ideal, std::string * query_condition, std::string * datetime, unsigned index);

#define NUMBER_NUMBERVECTORS 1
const char* NUMBERVECTORS[NUMBER_NUMBERVECTORS] = { "clique" };
std::vector<const char*> COLUMNVECTORS[NUMBER_NUMBERVECTORS] = { { "cliqueNumber", "maxCliqueNumber" } };
Graph_numbers GETTERS[NUMBER_NUMBERVECTORS] = { &Graph::get_clique_numbers };

#define NUMBER_TYPES 6
const char* TYPES[NUMBER_TYPES] = { "connected", "cograph", "euler", "chordal", "claw-free", "closed" };
const char* PRINT_NAMES[NUMBER_TYPES] = { "connected graphs", "cographs", "euler graphs", "chordal graphs", "claw-free graphs", "closed graphs" };
Graph_test TESTS[NUMBER_TYPES] = { &Graph::is_connected, &Graph::is_cograph, &Graph::is_euler, &Graph::is_chordal, &Graph::is_clawfree, &Graph::is_closed };
const char* STD_CONDITIONS[NUMBER_TYPES] = { 0, 0, 0, 0, 0, "type LIKE '%chordal%' AND type LIKE '%claw-free%'" };

#define NUMBER_LABELINGS 1
const char* LABELINGS[NUMBER_LABELINGS] = { "closed" };
Gen_labeling GENERATORS[NUMBER_LABELINGS] = { &Graph::gen_closed_labeling };

#define NUMBER_RESULTS 2
const char* RESULTS[NUMBER_RESULTS] = { "betti", "hpoldeg" };
Result_inserter INSERTERS[NUMBER_RESULTS] = { &DatabaseInterface::insert_betti_data, &DatabaseInterface::insert_hpoldeg_data };


//########## IO functions ##########
/**
 * gets the user input and returns the interaction key based on the keyword of the input
**/
int io_interface(std::string * input) {
	INPUT("");

	if (!getline(std::cin, *input))
		return 0;

	SEPARATE();

	size_t cut_index = input->find_first_of(' ');
	std::string keyword = input->substr(0, cut_index);
	int key = -2;

	if (keyword == "SELECT"
		|| keyword == "Select"
		|| keyword == "select")
		key = -1;
	else if (keyword == "exit"
		|| keyword == "quit")
		key = 0;
	else if (keyword == "help")
		key = 1;
	else if (keyword == "show")
		key = 2;
	else if (keyword == "save")
		key = 3;
	else if (keyword == "insert")
		key = 4;
	else if (keyword == "label")
		key = 5;
	else if (keyword == "result")
		key = 6;
	else if (keyword == "scripts")
		key = 7;
	else if (keyword == "numbers")
		key = 8;

	if (key > 0)
	{
		if (cut_index < input->length() - 1)
			*input = input->substr(cut_index + 1, std::string::npos);
		else
			*input = "";
	}

	return key;
}


/**
 * outputs help texts to the terminal
**/
void help_parse(DatabaseInterface * dbi, std::string * input) {
	bool sql		= false;
	bool insert		= false;
	bool numbers	= false;
	bool label		= false;
	bool scripts	= false;
	bool result		= false;
	bool show		= false;
	bool save		= false;

	int argc = 0;

	while (!input->empty())
	{
		size_t cut_index;
		std::string arg;
		
		cut_index = input->find_first_of(' ');
		arg = input->substr(0, cut_index);
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
		else if (arg == "result")
			result = true;
		else if (arg == "show")
			show = true;
		else if (arg == "save")
			save = true;
		else{
			INVALID_ARG();
			return;
		}

		if (cut_index < input->length() - 1)
			*input = input->substr(cut_index + 1, std::string::npos);
		else
			*input = "";
	}

	if (argc == 0)
		std::cout << "\n" << help_text << std::endl;
	else if (argc == 1)
	{
		if (sql)
			std::cout << "\n" << sql_text << std::endl;
		else if (insert)
			std::cout << "\n" << insert_text << std::endl;
		else if (numbers)
			std::cout << "\n" << numbers_text << std::endl;
		else if (label)
			std::cout << "\n" << label_text << std::endl;
		else if (scripts)
			std::cout << "\n" << scripts_text << std::endl;
		else if (result)
			std::cout << "\n" << result_text << std::endl;
		else if (show)
			std::cout << "\n" << show_text << std::endl;
		else if (save)
			std::cout << "\n" << save_text << std::endl;
	}
	else
		PARSE_ERROR("Too many arguments.");
}


/**
 * parses the arguments for show to decide the output format and the limit for the number of output rows, then chooses the appropriate function
**/
void show_parse(DatabaseInterface * dbi, std::string * input) {
	int limit = -1;
	bool force = false;
	bool rich = false;

	while (!input->empty())
	{
		size_t cut_index;
		std::string arg;

		if (input->front() == '"')
		{
			arg = input->substr(1, std::string::npos);
			cut_index = arg.find_first_of('"');

			if (cut_index == std::string::npos)
			{
				PARSE_ERROR("'\"' missing.");
				return;
			}

			arg = arg.substr(0, cut_index);
			cut_index += 2;
		}
		else
		{
			cut_index = input->find_first_of(' ');
			arg = input->substr(0, cut_index);
		}

		if (arg == "-r")
			rich = true;
		else if (arg == "-f")
		{
			if (limit != -1)
			{
				WARNING(arg << " will be ignored since a limit was already specified.");
				INPUT("Proceed ? (y / n)");

				std::string extra_input;
				while (true)
				{
					getline(std::cin, extra_input);
					if (extra_input == "y")
						break;
					if (extra_input == "n")
						return;
					PARSE_ERROR("Not a valid input.");
					INPUT("");
				}
				SEPARATE();
			}
			else
				force = true;
		}
		else if (force)
		{
			WARNING(arg << " will be ignored since -f was given as an argument.");
			INPUT("Proceed ? (y / n)");

			std::string extra_input;
			while (true)
			{
				getline(std::cin, extra_input);
				if (extra_input == "y")
					break;
				if (extra_input == "n")
					return;
				PARSE_ERROR("Not a valid input.");
				INPUT("");
			}
			SEPARATE();
		}
		else
		{
			limit = 0;
			for (unsigned i = 1; i < arg.length(); i++)
			{
				if (arg.at(i) < '0'
					|| arg.at(i) > '9')
				{
					INVALID_ARG();
					return;
				}

				limit *= 10;
				limit += arg.at(i) - '0';
			}
		}

		if (cut_index < input->length() - 1)
			*input = input->substr(cut_index + 1, std::string::npos);
		else
			*input = "";
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
	std::string file_name = "database.view";
	bool rich = false;
	bool visualisation = false;
	bool g6 = false;

	while (!input->empty())
	{
		size_t cut_index;
		std::string arg;

		if (input->front() == '"')
		{
			arg = input->substr(1, std::string::npos);
			cut_index = arg.find_first_of('"');

			if (cut_index == std::string::npos)
			{
				PARSE_ERROR("'\"' missing.");
				FAIL("Saving current view", "");
				return;
			}

			arg = arg.substr(0, cut_index);
			cut_index += 2;
		}
		else
		{
			cut_index = input->find_first_of(' ');
			arg = input->substr(0, cut_index);
		}

		if (arg == "-r")
		{
			if (visualisation
				|| g6)
				WARNING(arg << " will be ignored since another view format was already specified.");
			else
			 rich = true;
		}
		else if (arg == "-v")
		{
			if (rich
				|| g6)
				WARNING(arg << " will be ignored since another view format was already specified.");
			else
				visualisation = true;
		}
		else if (arg == "-g6")
		{
			if (rich
				|| visualisation)
				WARNING(arg << " will be ignored since another view format was already specified.");
			else
				g6 = true;
		}
		else{
			file_name = arg + ".view";
			std::ifstream test(file_name);

			if (test.is_open())
			{
				WARNING(file_name << " already exists.");
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
					PARSE_ERROR("Not a valid input.");
					INPUT("");
				}
			}

			test.close();
		}

		if (cut_index < input->length() - 1)
			*input = input->substr(cut_index + 1, std::string::npos);
		else
			*input = "";
	}

	std::ofstream file(file_name, std::ios::trunc);

	if (!file.is_open())
	{
		FAIL("Saving view", "Unable to open " << file_name << ".");
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
		RESULT("Saved graphs in current view to " << file_name << ".");
	else
		RESULT("Saved current view to " << file_name << ".");
}


/**
* parses the arguments for insert
**/
void insert_parse(DatabaseInterface * dbi, std::string * input) {
	FORMAT format = FORMAT::NONE;
	std::string filename = "";

	while (!input->empty())
	{
		size_t cut_index;
		std::string arg;

		if (input->front() == '"')
		{
			arg = input->substr(1, std::string::npos);
			cut_index = arg.find_first_of('"');

			if (cut_index == std::string::npos)
			{
				PARSE_ERROR("Second '\"' missing.");
				FAIL("Inserting graphs", "");
				return;
			}

			arg = arg.substr(0, cut_index);
			cut_index += 2;
		}
		else
		{
			cut_index = input->find_first_of(' ');
			arg = input->substr(0, cut_index);
		}

		if (arg == "-g6")
		{
			if (format == FORMAT::NONE)
				format = FORMAT::G6;
			else
			{
				PARSE_ERROR("Format already specified.");
				FAIL("Inserting graphs", "");
				return;
			}
		}
		else if (arg == "-list")
		{
			if (format == FORMAT::NONE)
				format = FORMAT::LIST;
			else
			{
				PARSE_ERROR("Format already specified.");
				FAIL("Inserting graphs", "");
				return;
			}
		}
		else
		{
			if (filename.empty())
				filename = arg;
			else
			{
				PARSE_ERROR("Either an argument is invalid or there are too many.");
				FAIL("Inserting graphs", "");
				return;
			}
		}

		if (cut_index < input->length() - 1)
			*input = input->substr(cut_index + 1, std::string::npos);
		else
			*input = "";
	}

	if (format == FORMAT::NONE)
	{
		FAIL("Inserting graphs", "No format specified.");
		return;
	}

	std::ifstream file(filename);
	if (file.is_open())
	{
		PROGRESS(3, "inserting graphs");
		dbi->insert_graphs(&file, format);
	}
	else
	{
		FAIL("Inserting graphs", "Unable to open '" << filename << "'.");
		return;
	}
	file.close();
}


/**
* parses the arguments for result
**/
void result_parse(DatabaseInterface * dbi, std::string * input) {
	int scriptID = -1;

	while (!input->empty())
	{
		size_t cut_index;
		std::string arg;

		cut_index = input->find_first_of(' ');
		arg = input->substr(0, cut_index);

		if (arg.front() != '-')
		{
			INVALID_ARG();
			FAIL("Adding result data", "");
			return;
		}

		if (scriptID == -1)
		{
			scriptID = 0;
			for (unsigned i = 1; i < arg.length(); i++)
			{
				if (arg.at(i) < '0'
					|| arg.at(i) > '9')
				{
					INVALID_ARG();
					FAIL("Adding result data", "");
					return;
				}

				scriptID *= 10;
				scriptID += arg.at(i) - '0';
			}
		}
		else
		{
			PARSE_ERROR("Too many arguments.");
			FAIL("Adding result data", "");
			return;
		}

		if (cut_index < input->length() - 1)
			*input = input->substr(cut_index + 1, std::string::npos);
		else
			*input = "";
	}

	if (scriptID == -1)
	{
		dbi->show_scripts();

		INPUT("Please select a scriptID to add the results of those M2 scripts.");

		std::string arg;
		if (!getline(std::cin, arg))
			return;

		SEPARATE();

		scriptID = 0;
		while (scriptID == 0)
		{
			for (unsigned i = 0; i < arg.length(); i++)
			{
				if (arg.at(i) < '0'
					|| arg.at(i) > '9')
				{
					PARSE_ERROR("'" << arg << "' is not a number.");
					INPUT("");

					if (!getline(std::cin, arg))
						return;

					SEPARATE();
					break;
				}

				scriptID *= 10;
				scriptID += arg.at(i) - '0';
			}
		}
	}

	std::string ideal, query_condition, datetime;
	int index = dbi->find_script_data(scriptID, &ideal, &query_condition, &datetime);

	if (index == -1)
		return;

	PROGRESS(1, "adding result data");
	if ((dbi->*INSERTERS[index])(&ideal, &query_condition, &datetime, index))
		dbi->execute_SQL_statement(&("DELETE FROM Scripts WHERE scriptID == " + std::to_string(scriptID)));
}


/**
 * parses the arguments for label
**/
void label_parse(DatabaseInterface * dbi, std::string * input) {
	bool allexcept = false;
	std::vector<bool> to_be_labeled;
	bool condition = false;
	std::string query_condition = "";

	for (int i = 0; i < NUMBER_TYPES; i++)
		to_be_labeled.push_back(false);

	while (!input->empty())
	{
		size_t cut_index;
		std::string arg;

		if (condition)
		{
			if (input->front() == '"')
			{
				arg = input->substr(1, std::string::npos);
				cut_index = arg.find_first_of('"');

				if (cut_index == std::string::npos)
				{
					PARSE_ERROR("Second '\"' missing.");
					FAIL("Labeling graphs", "");
					return;
				}

				query_condition = arg.substr(0, cut_index);
				cut_index += 2;
			}
			else
			{
				PARSE_ERROR("'-where' must be followed by an SQL expression in quotation marks (\"\").");
				FAIL("Labeling graphs", "");
				return;
			}

			condition = false;
		}
		else
		{
			cut_index = input->find_first_of(' ');
			arg = input->substr(0, cut_index);

			if (arg == "-allexcept")
				allexcept = true;
			else if (arg == "-where")
				condition = true;
			else
			{
				int i;
				for (i = 0; i < NUMBER_TYPES; i++)
				{
					if (arg == "-" + std::string(TYPES[i]))
					{
						to_be_labeled[i] = true;
						break;
					}
				}

				if (i >= NUMBER_TYPES)
				{
					INVALID_ARG();
					FAIL("Labeling graphs", "");
					return;
				}
			}
		}

		if (cut_index < input->length() - 1)
			*input = input->substr(cut_index + 1, std::string::npos);
		else
			*input = "";
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
		size_t cut_index;
		std::string arg;

		if (condition)
		{
			if (input->front() == '"')
			{
				arg = input->substr(1, std::string::npos);
				cut_index = arg.find_first_of('"');

				if (cut_index == std::string::npos)
				{
					PARSE_ERROR("Second '\"' missing.");
					FAIL("Computing numbers", "");
					return;
				}

				query_condition = arg.substr(0, cut_index);
				cut_index += 2;
			}
			else
			{
				PARSE_ERROR("'-where' must be followed by an SQL expression in quotation marks (\"\").");
				FAIL("Computing numbers", "");
				return;
			}

			condition = false;
		}
		else
		{
			cut_index = input->find_first_of(' ');
			arg = input->substr(0, cut_index);

			if (arg == "-allexcept")
				allexcept = true;
			else if (arg == "-where")
				condition = true;
			else
			{
				int i;
				for (i = 0; i < NUMBER_NUMBERVECTORS; i++)
				{
					if (arg == "-" + std::string(NUMBERVECTORS[i]))
					{
						to_be_computed[i] = true;
						break;
					}
				}

				if (i >= NUMBER_NUMBERVECTORS)
				{
					INVALID_ARG();
					FAIL("Computing numbers", "");
					return;
				}
			}
		}

		if (cut_index < input->length() - 1)
			*input = input->substr(cut_index + 1, std::string::npos);
		else
			*input = "";
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
			PROGRESS(1, "computing " << NUMBERVECTORS[i]);

			dbi->update_numbers(GETTERS[i], &(COLUMNVECTORS[i]), query_condition.empty() ? 0 : query_condition.c_str());
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
	int index = -1;
	std::string idealname = "";
	std::string filename = "";
	std::string query_condition = "";

	while (!input->empty())
	{
		size_t cut_index;
		std::string arg;

		if (condition)
		{
			if (input->front() == '"')
			{
				arg = input->substr(1, std::string::npos);
				cut_index = arg.find_first_of('"');

				if (cut_index == std::string::npos)
				{
					PARSE_ERROR("'\"' missing.");
					FAIL("Generating M2 scripts", "");
					return;
				}

				query_condition = arg.substr(0, cut_index);
				cut_index += 2;
			}
			else
			{
				PARSE_ERROR("'-where' must be followed by an SQL expression in quotation marks (\"\").");
				FAIL("Generating M2 scripts", "");
				return;
			}

			condition = false;
		}
		else
		{
			if (input->front() == '"')
			{
				arg = input->substr(1, std::string::npos);
				cut_index = arg.find_first_of('"');

				if (cut_index == std::string::npos)
				{
					PARSE_ERROR("Second '\"' missing.");
					FAIL("Generating M2 scripts", "");
					return;
				}

				arg = arg.substr(0, cut_index);
				cut_index += 2;
			}
			else
			{
				cut_index = input->find_first_of(' ');
				arg = input->substr(0, cut_index);
			}

			if (arg == "-where")
				condition = true;
			else if (arg.front() == '-')
			{
				bool match = false;

				for (int i = 0; i < NUMBER_LABELINGS; i++)
				{
					if (arg == "-" + std::string(LABELINGS[i]))
					{
						labeling = i;
						match = true;
						break;
					}
				}

				if (!match && index == -1)
				{
					for (int i = 0; i < NUMBER_RESULTS; i++)
					{
						if (arg == "-" + std::string(RESULTS[i]))
						{
							index = i;
							match = true;
							break;
						}
					}
				}

				if (!match && batch_size == -1)
				{
					batch_size = 0;
					for (unsigned i = 1; i < arg.length(); i++)
					{
						if (arg.at(i) < '0'
							|| arg.at(i) > '9')
							break;

						batch_size *= 10;
						batch_size += arg.at(i) - '0';
					}
					match = true;
				}

				if (!match)
				{
					PARSE_ERROR("Either an argument is invalid or there are too many.");
					FAIL("Generating M2 scripts", "");
					return;
				}
			}
			else if (idealname.empty())
				idealname = arg;
			else if (filename.empty())
				filename = arg;
			else
			{
				PARSE_ERROR("Too many arguments.");
				FAIL("Generating M2 scripts", "");
				return;
			}
		}

		if (cut_index < input->length() - 1)
			*input = input->substr(cut_index + 1, std::string::npos);
		else
			*input = "";
	}

	if (idealname.empty())
	{
		FAIL("Generating M2 scripts", "No ideal name specified.");
		return;
	}

	if (index == -1)
	{
		FAIL("Generating M2 scripts", "No result type specified.");
		return;
	}

	if (idealname.find("Bettis") == std::string::npos)
		dbi->generate_m2_scripts(&idealname, labeling == -1 ? 0 : GENERATORS[labeling], batch_size == -1 ? 2500 : batch_size, query_condition.empty() ? 0 : query_condition.c_str(), filename.empty() ? 0 : filename.c_str(), labeling == -1 ? 0 : LABELINGS[labeling], index);
	else
		FAIL("Generating M2 scripts", "Please choose an ideal name other than " << idealname << " as anything with 'Bettis' in it would interfere with the database.");
}


//########## main function ##########
/**
 * opens the given database (if none is specified it opens 'Graphs.db'),
 * gets the interaction key via io_interface and takes action based on the key
**/
int main(int argc, char* argv[]) {
	std::cout << help_text << std::endl;

	std::string input = "";
	DatabaseInterface dbi;

	if (argc == 2)
		dbi = DatabaseInterface(argv[1]);
	else if (argc > 2)
	{
		std::cerr << "Too many arguments." << std::endl;
		return 1;
	}
	else
		dbi = DatabaseInterface("Graphs.db");

	dbi.create_scripts_table();
	dbi.create_graphs_table();

	while (true)
	{
		int key = io_interface(&input);

		switch (key)
		{
		case 0: return 0;
		case -2: dbi.execute_SQL_statement(&input); break;
		case -1: dbi.execute_SQL_query(&input); break;
		case 1: help_parse(&dbi, &input); break;
		case 2: show_parse(&dbi, &input); break;
		case 3: save_parse(&dbi, &input); break;
		case 4: insert_parse(&dbi, &input); break;
		case 5: label_parse(&dbi, &input); break;
		case 6: result_parse(&dbi, &input); break;
		case 7: script_parse(&dbi, &input); break;
		case 8: numbers_parse(&dbi, &input); break;
		default: break;
		}
	}
}