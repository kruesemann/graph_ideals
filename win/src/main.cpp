#include "stdafx.h"
#include "parser.h"


//########## IO function ##########
/**
 * gets the user input and calls the appropriate parser based on the keyword of the input
**/
int io_interface(DatabaseInterface * dbi) {
	std::string input;
	INPUT("");

	if (!getline(std::cin, input))
		return 0;

	SEPARATE();

	std::string keyword = cut_first_argument(&input);

	if (keyword == "exit"
		|| keyword == "quit")
		return 0;

	if (keyword == "SELECT"
		|| keyword == "Select"
		|| keyword == "select")
	{
		std::string query = "select " + input;
		dbi->execute_SQL_query(&query);
	}
	else if (keyword == "help")
		help_parse(dbi, &input);
	else if (keyword == "show")
		show_parse(dbi, &input);
	else if (keyword == "save")
		save_parse(dbi, &input);
	else if (keyword == "import")
		import_parse(dbi, &input);
	else if (keyword == "classify")
		classify_parse(dbi, &input);
	else if (keyword == "results")
		results_parse(dbi, &input);
	else if (keyword == "scripts")
		script_parse(dbi, &input);
	else if (keyword == "compute")
		compute_parse(dbi, &input);
	else
	{
		std::string statement = keyword + " " + input;
		dbi->execute_SQL_statement(&statement);
	}

	return 1;
}


//########## main function ##########
/**
 * opens the given database (if none is specified it opens 'Graphs.db'),
 * creates scripts and graphs table, if they do not exist
 * prints help text and calls io_interface
**/
int main(int argc, char * argv[]) {
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

	std::string temp = "";
	help_parse(&dbi, &temp);
	while (io_interface(&dbi));
}