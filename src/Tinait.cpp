#include <readline/readline.h>
#include <readline/history.h>

#include "stdafx.h"
#include "parser.h"


const std::string vocabulary[] = {
    "exit", "quit", "help", "sql", "import", "compute", "classify", "scripts", "results", "show", "save",
    "select", "distinct", "from", "where", "group by", "order by", "desc", "asc", "limit", "and", "case", "when", "then", "else", "end", "max", "count", "sum", "like", "not", "null", "is null", "union", "inner join",
    "graphs", "graphid", "graphorder", "graphsize", "edges", "type", "cliquenr", "maxcliques", "independencenr", "maxindependentsets", "detournr", "mindeg", "maxdeg", "girth", "connected", "cograph", "euler", "chordal", "claw-free", "closed", "cone",
    "-g6", "-list", "-clique", "-independence", "-detour", "-degree", "-girth", "-where", "-allexcept", "-connected", "-cograph", "-euler", "-chordal", "-claw-free", "-closed", "-cone", "-betti", "-hpoldeg", "bei", "initBei", "clInitBei", "pbei", "initPbei"
};


//########## IO function ##########
/**
* generates the the matches in the vocabulary of keywords and (longer) arguments for text
**/
char * completion_generator(const char * text, int state) {
	static std::vector<std::string> matches;
	static size_t match_index = 0;

	if (state == 0)
	{
		matches.clear();
		match_index = 0;

		std::string input = std::string(text);
		for (std::string word : vocabulary)
		{
			if (word.size() >= input.size()
				&& word.compare(0, input.size(), input) == 0)
				matches.push_back(word);
		}
	}

	if (match_index >= matches.size())
		return 0;

	return strdup(matches[match_index++].c_str());
}


/**
* attempts to match the given text against the vocabulary of keywords and (longer) arguments
**/
char ** dbi_completion (const char * text, int start, int end) {
	return rl_completion_matches(text, completion_generator);
}


/**
 * gets the user input and calls the appropriate parser based on the keyword of the input
**/
int io_interface(DatabaseInterface * dbi) {
	std::string input;
    
    	char * buffer = readline("\n>> ");

	if(!buffer)
	{
		std::cout << std::endl;
		return 0;
	}
	
	input = buffer;

	if(!input.empty())
		add_history(buffer);

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
	rl_attempted_completion_function = dbi_completion;

	std::string temp = "";
	help_parse(&dbi, &temp);
	while (io_interface(&dbi));
}
