#include <iostream>

#include "stdafx.h"
#include "parser.h"
#include "test.h"


//########## IO function ##########
/**
 * gets the user input and returns the interaction key based on the keyword of the input
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
		dbi->execute_SQL_query(&(keyword + " " + input));
	else if (keyword == "help")
		help_parse(dbi, &input);
	else if (keyword == "show")
		show_parse(dbi, &input);
	else if (keyword == "save")
		save_parse(dbi, &input);
	else if (keyword == "import")
		import_parse(dbi, &input);
	else if (keyword == "type")
		type_parse(dbi, &input);
	else if (keyword == "results")
		results_parse(dbi, &input);
	else if (keyword == "scripts")
		script_parse(dbi, &input);
	else if (keyword == "numbers")
		values_parse(dbi, &input);
	else
		dbi->execute_SQL_statement(&(keyword + " " + input));

	return 1;
}


//########## main function ##########
/**
 * opens the given database (if none is specified it opens 'Graphs.db'),
 * gets the interaction key via io_interface and takes action based on the key
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

	complete_test(&dbi);

	//help_parse(&dbi, &input);
	while (io_interface(&dbi));
}