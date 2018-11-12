#include "DatabaseInterface.h"

#include <ctime>


#define PI 3.14159265
#define max_column_width 66

//########## helper functions ##########
/**
 * converts str to unsigned
**/
inline unsigned str_to_unsigned(std::string * str) {
	unsigned number = 0;
	for (unsigned i = 0; i < str->length(); i++)
	{
		number *= 10;
		number += str->at(i) - '0';
	}
	return number;
}


/**
* returns current date and time as a string
**/
inline std::string cdatetime() {
	time_t rawtime;
	struct tm timeinfo;
	char buffer[80];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &timeinfo);
	std::string str(buffer);

	return str;
}


/**
 * returns the appropriate coordinates for a vertex to print the graph of given order on a circle
**/
std::pair<int, int> get_coordinate(unsigned vertex, unsigned order) {
	int first = (int)(100 * cos(2 * (vertex - 1) * PI / order));
	int second = (int)(100 * sin(2 * (vertex - 1) * PI / order));

	return std::pair<int, int>(first, second);
}


//########## public member functions ##########
/**
 * resets all member variables excepts the database itself
**/
void DatabaseInterface::reset_view() {
	number_columns = 0;
	number_rows = 0;
	view_columns = {};
	view_contents = {};
	column_widths = {};
}


/**
 * outputs the current view to the terminal by iterating over all rows (until the limit is reached) in the current view
**/
void DatabaseInterface::show_view(int limit) {
	if (number_columns != 0)
	{
		for (unsigned j = 0; j < number_columns; j++)
			std::cout << std::string(column_widths[j], '_');
		std::cout << "\n";

		for (unsigned j = 0; j < number_columns - 1; j++)
			std::cout << view_columns[j] << std::string(column_widths[j] - view_columns[j].length(), ' ');
		std::cout << view_columns[number_columns - 1] << "\n";

		for (unsigned j = 0; j < number_columns; j++)
			std::cout << std::string(column_widths[j], '_');
		std::cout << "\n";

		for (int i = 0; i < (int)number_rows; i++)
		{
			if (limit != -1
				&& i >= limit)
			{
				std::cout << "\n ... [View exceeds " << limit << " rows. For the complete list please save as text file or show view with higher limit.]\n";
				break;
			}

			for (unsigned j = 0; j < number_columns - 1; j++)
				std::cout << view_contents[i][j] << std::string(column_widths[j] - view_contents[i][j].length(), ' ');
			std::cout << view_contents[i][number_columns - 1] << "\n";
		}
	}

	std::cout << std::endl;
}


/**
* outputs the current view to given file by iterating over all rows in the current view
**/
void DatabaseInterface::save_view(std::ofstream * file) {
	if (number_columns != 0)
	{
		for (unsigned j = 0; j < number_columns; j++)
			*file << std::string(column_widths[j], '_');
		*file << "\n";

		for (unsigned j = 0; j < number_columns - 1; j++)
			*file << view_columns[j] << std::string(column_widths[j] - view_columns[j].length(), ' ');
		*file << view_columns[number_columns - 1] << "\n";

		for (unsigned j = 0; j < number_columns; j++)
			*file << std::string(column_widths[j], '_');
		*file << "\n";

		for (unsigned i = 0; i < number_rows; i++)
		{
			for (unsigned j = 0; j < number_columns - 1; j++)
				*file << view_contents[i][j] << std::string(column_widths[j] - view_contents[i][j].length(), ' ');
			*file << view_contents[i][number_columns - 1] << "\n";
		}
	}
}


/**
* outputs the current view to the terminal in rich format by iterating over all rows (until the limit is reached) in the current view
* all data with 'Bettis' in its column name will be printed as tables
**/
void DatabaseInterface::show_view_rich(int limit) {
	if (number_columns != 0)
	{
		std::vector<unsigned> matrix_indices;
		std::vector<std::string> table_names;
		std::string columns = "";

		unsigned last_whitespace = 0;
		unsigned overall_width = 0;

		for (unsigned j = 0; j < number_columns; j++)
		{
			std::string column = view_columns[j];

			if (column.find("Bettis") != std::string::npos)
			{
				matrix_indices.push_back(j);
				table_names.push_back(column);
			}
			else
			{
				columns += std::string(last_whitespace, ' ') + column;
				last_whitespace = column_widths[j] - view_columns[j].length();
				overall_width += column_widths[j];
			}
		}

		unsigned index;

		for (int i = 0; i < (int)number_rows; i++)
		{
			if (limit != -1
				&& i >= limit)
			{
				std::cout << "\n ... [View exceeds " << limit << " rows. For the complete list please save as text file or show view with higher limit.]\n";
				break;
			}

			std::cout << std::string(overall_width, '_') << "\n" << columns << "\n";

			index = 0;
			std::vector<BettiTable> tables;
			std::string row = "";

			last_whitespace = 0;

			for (unsigned j = 0; j < number_columns; j++)
			{
				if (index < matrix_indices.size() && j == matrix_indices[index])
				{
					BettiTable b;
					b.read_from_line(&view_contents[i][j]);
					tables.push_back(b);
					index++;
				}
				else
				{
					row += std::string(last_whitespace, ' ') + view_contents[i][j];
					last_whitespace = column_widths[j] - view_contents[i][j].length();
				}
			}
			std::cout << row << "\n";

			if (!tables.empty())
				std::cout << "\n";

			std::cout << BettiTable::convert_tables_to_string(&tables, &table_names);
		}
	}

	std::cout << std::endl;
}


/**
* outputs the current view to given file in rich format by iterating over all rows in the current view
* all data with 'Bettis' in its column name will be printed as tables
**/
void DatabaseInterface::save_view_rich(std::ofstream * file) {
	if (number_columns != 0)
	{
		std::vector<unsigned> matrix_indices;
		std::vector<std::string> table_names;
		std::string columns = "";

		unsigned last_whitespace = 0;
		unsigned overall_width = 0;

		for (unsigned j = 0; j < number_columns; j++)
		{
			std::string column = view_columns[j];

			if (column.find("Bettis") != std::string::npos)
			{
				matrix_indices.push_back(j);
				table_names.push_back(column);
			}
			else
			{
				columns += std::string(last_whitespace, ' ') + column;
				last_whitespace = column_widths[j] - view_columns[j].length();
				overall_width += column_widths[j];
			}
		}

		unsigned index;

		for (int i = 0; i < (int)number_rows; i++)
		{
			*file << std::string(overall_width, '_') << "\n" << columns << "\n";

			index = 0;
			std::vector<BettiTable> tables;
			std::string row = "";

			last_whitespace = 0;

			for (unsigned j = 0; j < number_columns; j++)
			{
				if (index < matrix_indices.size() && j == matrix_indices[index])
				{
					BettiTable b;
					b.read_from_line(&view_contents[i][j]);
					tables.push_back(b);
					index++;
				}
				else
				{
					row += std::string(last_whitespace, ' ') + view_contents[i][j];
					last_whitespace = column_widths[j] - view_contents[i][j].length();
				}
			}
			*file << row << "\n";

			if (!tables.empty())
				*file << "\n";

			*file << BettiTable::convert_tables_to_string(&tables, &table_names);
		}
	}
}


/**
 * outputs all graphs of the current view to given file in LaTeX-TikZ format (also outputs necessary LaTeX-package includes) by iterating over all rows in the current view
**/
void DatabaseInterface::save_view_visualisation(std::ofstream * file) {
	int graphID_index = -1;
	int graphOrder_index = -1;
	int edges_index = -1;

	for (unsigned i = 0; i < number_columns; i++)
	{
		if (view_columns[i] == "graphID")
		{
			if (graphID_index == -1)
				graphID_index = i;
			else
			{
				FAIL("Saving view", "Visualisation does not support multiple graphs per row.");
				return;
			}
		}
		else if (view_columns[i] == "graphOrder")
		{
			if (graphOrder_index == -1)
				graphOrder_index = i;
			else
			{
				FAIL("Saving view", "Visualisation does not support multiple graphs per row.");
				return;
			}
		}
		else if (view_columns[i] == "edges")
		{
			if (edges_index == -1)
				edges_index = i;
			else
			{
				FAIL("Saving view", "Visualisation does not support multiple graphs per row.");
				return;
			}
		}
	}

	if (graphOrder_index == -1
		|| edges_index == -1)
	{
		FAIL("Saving view", "View does not include both graphOrder and edges. These are necessary to visualize the graphs.");
		return;
	}

	*file << "LaTeX code snippets for visualisation of graphs with TikZ\n" \
		"\n%%########## includes ##########\n\n" \
		"\\usepackage{tikz}\n\\usetikzlibrary{ decorations.pathreplacing }\n\\usetikzlibrary{ shapes.misc }\n\\usetikzlibrary{ calc }\n" \
		"\n%%########## document code ##########\n\n";

	for (unsigned i = 0; i < number_rows; i++)
	{
		std::string info = "";
		if (graphID_index != -1)
			info += view_columns[graphID_index] + ": " + view_contents[i][graphID_index] + ", ";
		else
			info += "Graph " + std::to_string(i + 1) + ", ";
		info += view_columns[graphOrder_index] + ": " + view_contents[i][graphOrder_index];
		info += "\n\n\\begin{center}\n\\begin{tikzpicture}[\nscale=0.04,mynode/.style={draw,fill=white,circle,outer sep=4pt,inner sep=2pt},myedge/.style={line width=1.5,black}\n]\n";
		*file << info;

		unsigned graphOrder;
		std::stringstream order_stream(view_contents[i][graphOrder_index]);
		order_stream >> graphOrder;

		Graph g(graphOrder, &view_contents[i][edges_index]);

		std::string coordinates = "";
		std::string edges = "";
		std::string vertices = "";

		for (unsigned v = 1; v <= graphOrder; v++)
		{
			std::pair<int, int> coordinate = get_coordinate(v, graphOrder);
			coordinates += "\\coordinate(p" + std::to_string(v) + ") at(" + std::to_string(coordinate.first) + "," + std::to_string(coordinate.second) + ");\n";

			for (unsigned w = v + 1; w <= graphOrder; w++)
			{
				if (g.adjacent(v, w))
					edges += "\\draw[myedge] (p" + std::to_string(v) + ") -- (p" + std::to_string(w) + ");\n";
			}

			vertices += "\\node[mynode] at (p" + std::to_string(v) + ") {};\n";
		}
		coordinates += "\n";
		edges += "\n";

		info = "\\end{tikzpicture}\n\\end{center}\n\n";
		if (i % 2 == 1) {
			info += "\\clearpage\n\n";
		}

		*file << coordinates << edges << vertices << info;
	}
}


/**
* outputs all graphs in the current view to given file in g6 format by iterating over all rows in the current view
**/
void DatabaseInterface::save_view_g6(std::ofstream * file) {
	int graphOrder_index = -1;
	int edges_index = -1;

	for (unsigned j = 0; j < number_columns; j++)
	{
		if (view_columns[j] == "graphOrder")
			graphOrder_index = j;
		else if (view_columns[j] == "edges")
			edges_index = j;
	}

	if (graphOrder_index == -1
		|| edges_index == -1)
	{
		FAIL("Saving view", "View does not include both graphOrder and edges. These are necessary to visualize the graphs.");
		return;
	}

	for (unsigned i = 0; i < number_rows; i++)
	{
		Graph g;
		g.read_graph_from_line(str_to_unsigned(&view_contents[i][graphOrder_index]), &view_contents[i][edges_index]);

		*file << g.convert_to_g6_format() << "\n";
	}
}


/**
* generates Macaulay2 scripts of all graphs satisfying query_condition, based on template filename, each labeled wrt to the labeling induced by ordering ordering_name
* for this, the function queries the database and iterates over all results printing the graphs into files based on the template
* function also registers the generated scripts in the scripts table so that results may be imported later
**/
void DatabaseInterface::generate_m2_scripts(std::string * name, unsigned * (Graph::*gen_ordering)(), unsigned batch_size, const char * query_condition, const char * filename, const char * ordering_name, unsigned index) {
	sqlite3_stmt * qry;

	if (query_condition)
	{
		if (sqlite3_prepare_v2(database, ("SELECT graphOrder,edges FROM Graphs WHERE " + std::string(query_condition)).c_str(), -1, &qry, 0) != SQLITE_OK)
		{
			FAIL("Generating M2 scripts", "SQL error: '" << "SELECT graphOrder,edges FROM Graphs WHERE " << query_condition << "' is an invalid query.");
			sqlite3_finalize(qry);
			return;
		}
	}
	else
	{
		if (sqlite3_prepare_v2(database, "SELECT graphOrder,edges FROM Graphs", -1, &qry, 0) != SQLITE_OK)
		{
			FAIL("Generating M2 scripts", "SQL error: 'SELECT graphOrder,edges FROM Graphs' is an invalid query.");
			sqlite3_finalize(qry);
			return;
		}
	}

	std::string default_filename = "template_" + *name + "_" + std::to_string(index) + ".m2";
	std::ifstream templ(filename ? filename : default_filename);

	if (!templ.is_open())
	{
		FAIL("Generating M2 scripts", "Unable to open '" << (filename ? filename : default_filename) << "'.");
		sqlite3_finalize(qry);
		return;
	}

	std::stringstream buffer;
	buffer << templ.rdbuf();
	templ.close();

	std::string datetime(cdatetime());

	unsigned k;
	unsigned i;

	for (k = 0; true; k++)
	{
		std::string script = "G = {\n";
		std::string edges;

		for (i = 0; i < batch_size && sqlite3_step(qry) == SQLITE_ROW; i++)
		{
			edges = (char *)sqlite3_column_text(qry, 1) ? (char *)sqlite3_column_text(qry, 1) : "ERROR";
			Graph g(sqlite3_column_int(qry, 0), &edges);

			if (gen_ordering)
			{
				unsigned * ordering = (g.*gen_ordering)();
				script += g.convert_to_string_wrt_ordering(ordering) + ",\n";
				delete[] ordering;
			}
			else
				script += g.convert_to_string() + ",\n";
		}



		script.pop_back();
		script.pop_back();
		script += "\n};\n\nfilename=\"" + std::to_string(index) + "_" + datetime + "_" + *name + "_" + std::to_string(k) + ".result\";\nN=" + std::to_string(i) + ";\n\n" + buffer.str();

		std::string filename = std::to_string(index) + "_" + datetime + "_" + *name + "_" + std::to_string(k) + ".m2";
		std::ofstream kFile(filename, std::ios::trunc);

		if (!kFile.is_open())
		{
			FAIL("Generating M2 scripts", "Unable to write to file '" << filename << "'.");
			sqlite3_finalize(qry);
			return;
		}

		kFile << script;

		kFile.close();
		
		PROGRESS(2, "generated '" << filename << "'");

		if (i < batch_size)
			break;
	}

	sqlite3_finalize(qry);

	if (k == 0 && i == 0)
	{
		FAIL("Generating M2 scripts", "Unable to find any graphs satisfying the condition of the query: '" << qry << "'.");
		return;
	}


	std::string columns = "INSERT INTO Scripts (name,";
	std::string values = "datetime,resultType) VALUES (\"" + *name + "\",";

	if (ordering_name)
	{
		columns += "ordering,";
		values += "\"" + std::string(ordering_name) + "\",";
	}

	columns += "batchsize,";
	values += std::to_string(batch_size) + ",";

	if (query_condition)
	{
		columns += "condition,";
		values += "\"" + std::string(query_condition) + "\",";
	}

	values += "\"" + datetime + "\"," + std::to_string(index) + ")";
	columns += values;

	execute_SQL_statement(&columns);
}


/**
* outputs the status table to the terminal
**/
void DatabaseInterface::show_scripts() {
	sqlite3_stmt * stmt;

	if (sqlite3_prepare_v2(database, "SELECT * FROM Scripts", -1, &stmt, 0) != SQLITE_OK)
	{
		SQL_ERROR("SELECT * FROM Scripts");
		sqlite3_finalize(stmt);
		return;
	}

	unsigned colc = sqlite3_column_count(stmt);
	std::vector<std::string> columns;
	std::vector<unsigned> widths;

	for (unsigned i = 0; i < colc; i++)
	{
		std::string column = sqlite3_column_name(stmt, i);
		columns.push_back(column);
		widths.push_back(column.length() + 3);
	}

	std::vector<std::vector<std::string>> contents;
	unsigned rowc = 0;

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		std::vector<std::string> row;
		for (unsigned i = 0; i < colc; i++)
		{
			std::string content;
			content = (char *)sqlite3_column_text(stmt, i) ? (char *)sqlite3_column_text(stmt, i) : "-";

			row.push_back(content);

			if (widths[i] < content.length() + 3)
				widths[i] = content.length() + 3;
		}
		contents.push_back(row);
		rowc++;
	}

	sqlite3_finalize(stmt);

	std::cout << "\n";

	unsigned last_whitespace = 0;
	for (unsigned j = 0; j < colc; j++)
	{
		std::cout << std::string(last_whitespace, ' ') << columns[j];
		last_whitespace = widths[j] - columns[j].length();
	}
	std::cout << "\n";

	for (unsigned i = 0; i < rowc; i++)
	{
		last_whitespace = 0;

		for (unsigned j = 0; j < colc; j++)
		{
			std::string content = contents[i][j];
			std::cout << std::string(last_whitespace, ' ') << content;
			last_whitespace = widths[j] - content.length();
		}
		std::cout << "\n";
	}

	std::cout << "\n" << std::endl;
}


/**
 * expects a SELECT SQL-statement
 * tries to execute query, save the result and output the view to the terminal
**/
bool DatabaseInterface::execute_SQL_query(std::string * query) {
	if (query->empty())
		return true;

	sqlite3_stmt * stmt;

	try
	{
		if (sqlite3_prepare_v2(database, query->c_str(), -1, &stmt, 0) != SQLITE_OK)
		{
			SQL_ERROR(*query);
			sqlite3_finalize(stmt);
			return false;
		}

		reset_view();

		number_columns = sqlite3_column_count(stmt);

		for (unsigned i = 0; i < number_columns; i++)
		{
			std::string column = sqlite3_column_name(stmt, i);
			view_columns.push_back(column);
			column_widths.push_back(column.length() + 3);
		}

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			std::vector<std::string> row;
			for (unsigned i = 0; i < number_columns; i++)
			{
				std::string content = (char *)sqlite3_column_text(stmt, i) ? (char *)sqlite3_column_text(stmt, i) : "-";
				row.push_back(content);

				if (column_widths[i] < content.length() + 3)
					column_widths[i] = content.length() + 3;
			}
			view_contents.push_back(row);
			number_rows++;
		}

		sqlite3_finalize(stmt);

		show_view();
	}
	catch (const char * error)
	{
		sqlite3_finalize(stmt);
		reset_view();
		FAIL("SQL query", error << ". Resetting view.");
		return false;
	}

	return true;
}


/**
 * tries to execute given statement without changing the current view
**/
bool DatabaseInterface::execute_SQL_statement(std::string * statement) {
	char * errMsg = 0;

	if (sqlite3_exec(database, statement->c_str(), 0, 0, &errMsg) != SQLITE_OK) {
		SQL_ERROR(*statement);
		RESULT(errMsg);
		sqlite3_free(errMsg);
		return false;
	}

	return true;
}


/**
* creates the script table in the database for ensuring database consistency
**/
bool DatabaseInterface::create_scripts_table() {
	std::string statement = "CREATE TABLE Scripts(" \
		"scriptID INTEGER PRIMARY KEY," \
		"name TEXT NOT NULL," \
		"ordering TEXT," \
		"batchsize INT NOT NULL," \
		"condition TEXT," \
		"datetime TEXT NOT NULL," \
		"resultType INT NOT NULL" \
		");";

	if (sqlite3_exec(database, statement.c_str(), 0, 0, 0) != SQLITE_OK)
		return false;

	return true;
}


/**
 * creates graphs table in the database
**/
bool DatabaseInterface::create_graphs_table() {
	std::string statement = "CREATE TABLE IF NOT EXISTS Graphs("  \
							"graphID INTEGER PRIMARY KEY," \
							"graphOrder INT NOT NULL," \
							"graphSize INT NOT NULL," \
							"edges TEXT NOT NULL," \
							"type TEXT" \
							");";

	return execute_SQL_statement(&statement);
}


/**
 * expects graphs in given file to be formatted correctly
 * batch imports the graphs into the database
**/
void DatabaseInterface::import_graphs(std::ifstream * file, bool (Graph::*Read_next_format)(std::ifstream * file)) {
	sqlite3_exec(database, "BEGIN TRANSACTION;", 0, 0, 0);

	unsigned j = 0;
	while (true)
	{
		unsigned i;
		Graph g;
		std::string statement = "INSERT INTO Graphs (graphOrder,graphSize,edges) VALUES ";

		for (i = 0; i < 10 && (g.*Read_next_format)(file); i++)
			statement += "(" + std::to_string(g.get_order()) + "," + std::to_string(g.get_size()) + ",'" + g.convert_to_string() + "'),";

		statement.pop_back();

		execute_SQL_statement(&statement);

		j += i;

		if (i < 10)
			break;
	}

	PROGRESS(2, j << " graphs imported");

	sqlite3_exec(database, "COMMIT;", 0, 0, 0);
}


/**
 * updates the type of all graphs that satisfy graph_test and query_condition by quering the database and iterating over all results
**/
bool DatabaseInterface::update_type(bool(Graph::*graph_test)(), const char * type, const char * query_condition) {
	std::string query = "SELECT graphID,graphOrder,edges,type FROM Graphs WHERE (type IS NULL OR type NOT LIKE '%" + std::string(type) + "%')";
	if (query_condition)
		query += " AND (" + std::string(query_condition) + ")";

	sqlite3_stmt * qry;

	if (sqlite3_prepare_v2(database, query.c_str(), -1, &qry, 0) != SQLITE_OK)
	{
		SQL_ERROR(query);
		FAIL("Classifying type", "");
		sqlite3_finalize(qry);
		return false;
	}

	sqlite3_stmt * stmt;

	if (sqlite3_prepare_v2(database, "UPDATE Graphs SET type = ? WHERE graphID == ?", -1, &stmt, 0) != SQLITE_OK)
	{
		SQL_ERROR("UPDATE Graphs SET type = ? WHERE graphID == ?");
		FAIL("Classifying type", "");
		sqlite3_finalize(qry);
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_exec(database, "BEGIN TRANSACTION;", 0, 0, 0);
	unsigned i;
	for (i = 1; sqlite3_step(qry) == SQLITE_ROW; i++)
	{
		std::string edges = ((char *)sqlite3_column_text(qry, 2));
		Graph g(sqlite3_column_int(qry, 1), &edges);

		if ((g.*graph_test)())
		{
			sqlite3_bind_text(stmt, 1, sqlite3_column_text(qry, 3) ? (std::string((char *)sqlite3_column_text(qry, 3)) + "," + std::string(type)).c_str() : type, -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 2, sqlite3_column_int(qry, 0));

			sqlite3_step(stmt);

			sqlite3_clear_bindings(stmt);
			sqlite3_reset(stmt);
		}

		if (i == 10000)
		{
			PROGRESS(2, i << " graphs tested");
			i = 1;
		}
	}
	sqlite3_exec(database, "COMMIT;", 0, 0, 0);

	sqlite3_finalize(qry);
	sqlite3_finalize(stmt);

	if (i > 1)
		PROGRESS(2, i - 1 << " graphs tested");
	else
	{
		FAIL("Classifying type", "Unable to find any graphs satisfying the condition of the query: '" << query << "'.");
		return false;
	}

	return true;
}


/**
* updates with graph_values the values in given columns of all graphs that satisfy query_condition by quering the database and iterating over all results
**/
bool DatabaseInterface::update_values(std::vector<unsigned>(Graph::*graph_values)(), std::vector<const char *> * columns, const char * query_condition) {
	if (columns->size() == 0)
	{
		FAIL("Computing values", "No value set specified.");
		return false;
	}

	for (unsigned i = 0; i < columns->size(); i++)
		sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + columns->at(i) + " INT;").c_str(), 0, 0, 0);

	std::string query = "SELECT graphID,graphOrder,edges FROM Graphs WHERE (" + std::string(columns->at(0)) + " IS NULL";
	for (unsigned i = 1; i < columns->size(); i++)
		query += " AND " + std::string(columns->at(i)) + " IS NULL";
	query += ")";

	if (query_condition)
		query += " AND (" + std::string(query_condition) + ")";

	sqlite3_stmt * qry;

	if (sqlite3_prepare_v2(database, query.c_str(), -1, &qry, 0) != SQLITE_OK)
	{
		SQL_ERROR(query);
		FAIL("Computing values", "");
		sqlite3_finalize(qry);
		return false;
	}

	std::string statement = "UPDATE Graphs SET " + std::string(columns->at(0)) + " = ?";
	for (unsigned i = 1; i < columns->size(); i++)
		statement += ", " + std::string(columns->at(i)) + " = ?";
	statement += " WHERE graphID == ?";

	sqlite3_stmt * stmt;

	if (sqlite3_prepare_v2(database, statement.c_str(), -1, &stmt, 0) != SQLITE_OK)
	{
		SQL_ERROR(statement);
		FAIL("Computing values", "");
		sqlite3_finalize(qry);
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_exec(database, "BEGIN TRANSACTION;", 0, 0, 0);
	unsigned i;
	for (i = 1; sqlite3_step(qry) == SQLITE_ROW; i++)
	{
		std::string edges = ((char *)sqlite3_column_text(qry, 2));
		Graph g(sqlite3_column_int(qry, 1), &edges);

		std::vector<unsigned> values = (g.*graph_values)();

		if (values.size() != columns->size())
		{
			FAIL("Computing values", "There are not the same amounts of values and columns.");
			sqlite3_exec(database, "COMMIT;", 0, 0, 0);
			sqlite3_finalize(qry);
			sqlite3_finalize(stmt);
			return false;
		}

		for (unsigned j = 0; j < values.size(); j++)
			sqlite3_bind_int(stmt, j + 1, values[j]);

		sqlite3_bind_int(stmt, values.size() + 1, sqlite3_column_int(qry, 0));

		sqlite3_step(stmt);

		sqlite3_clear_bindings(stmt);
		sqlite3_reset(stmt);

		if (i == 10000)
		{
			PROGRESS(2, i << " graphs updated");
			i = 1;
		}
	}
	sqlite3_exec(database, "COMMIT;", 0, 0, 0);

	sqlite3_finalize(qry);
	sqlite3_finalize(stmt);

	if (i > 1)
		PROGRESS(2, i - 1 << " graphs updated");
	else
	{
		FAIL("Computing values", "Unable to find any graphs satisfying the condition of the query: '" << query << "'.");
		return false;
	}

	return true;
}


/**
* returns the internal result type of the script with ID scriptID in the scripts table
* changes the values of name, query_condition and datetime to the respective values in the scripts table
**/
unsigned DatabaseInterface::find_script_data(unsigned scriptID, std::string * name, std::string * query_condition, std::string * datetime) {
	sqlite3_stmt * qry1;
	if (sqlite3_prepare_v2(database, ("SELECT name,condition,datetime,resultType FROM Scripts WHERE scriptID == " + std::to_string(scriptID)).c_str(), -1, &qry1, 0) != SQLITE_OK)
	{
		SQL_ERROR("SELECT name,condition,datetime,resultType FROM Scripts WHERE scriptID == " << scriptID);
		FAIL("Adding Betti data", "");
		sqlite3_finalize(qry1);
		return -1;
	}

	if (sqlite3_step(qry1) != SQLITE_ROW)
	{
		FAIL("Adding Betti data", "There is no open script with scriptID " << scriptID << " in the Scripts table.");
		sqlite3_finalize(qry1);
		return -1;
	}

	*name = (char *)sqlite3_column_text(qry1, 0);
	*query_condition = (char *)sqlite3_column_text(qry1, 1) ? (char *)sqlite3_column_text(qry1, 1) : "";
	*datetime = (char *)sqlite3_column_text(qry1, 2);
	unsigned index = sqlite3_column_int(qry1, 3);
	sqlite3_finalize(qry1);

	return index;
}


/**
 * reads all Betti tables from the files determined by the given name, datetime and index, updates all graphs satisfying query_condition
**/
bool DatabaseInterface::insert_betti_data(std::string * name, std::string * query_condition, std::string * datetime, unsigned index) {
	std::string column_name = *name;
	size_t cut_index = column_name.find_first_of(' ');
	while (cut_index < column_name.length() - 1)
		column_name = column_name.substr(0, cut_index) + column_name.substr(cut_index + 1, std::string::npos);

	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + *name + "Bettis TEXT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + *name + "PD INT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + *name + "Reg INT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + *name + "Extremals TEXT;").c_str(), 0, 0, 0);

	sqlite3_stmt * qry2;
	if (query_condition->empty())
	{
		if (sqlite3_prepare_v2(database, "SELECT graphID FROM Graphs", -1, &qry2, 0) != SQLITE_OK)
		{
			SQL_ERROR("SELECT graphID FROM Graphs");
			FAIL("Adding Betti data", "");
			sqlite3_finalize(qry2);
			return false;
		}
	}
	else
	{
		if (sqlite3_prepare_v2(database, ("SELECT graphID FROM Graphs WHERE " + *query_condition).c_str(), -1, &qry2, 0) != SQLITE_OK)
		{
			SQL_ERROR("SELECT graphID FROM Graphs" << *query_condition);
			FAIL("Adding Betti data", "");
			sqlite3_finalize(qry2);
			return false;
		}
	}

	std::string statement = "UPDATE Graphs SET " + *name + "Bettis = ?, " + *name + "PD = ?, " + *name + "Reg = ?, " + *name + "Extremals = ? WHERE graphID == ?;";
	sqlite3_stmt * stmt1;

	if (sqlite3_prepare_v2(database, statement.c_str(), -1, &stmt1, 0) != SQLITE_OK)
	{
		SQL_ERROR(statement);
		FAIL("Adding Betti data", "");
		sqlite3_finalize(qry2);
		sqlite3_finalize(stmt1);
		return false;
	}

	sqlite3_exec(database, "BEGIN TRANSACTION;", 0, 0, 0);
	unsigned count = 0;
	for (unsigned k = 0; true; k++)
	{
		std::string filename = std::to_string(index) + "_" + *datetime + "_" + *name + "_" + std::to_string(k) + ".result";
		std::ifstream kFile(filename);

		if (!kFile.is_open())
		{
			if (k == 0)
			{
				FAIL("Adding Betti data", "Unable to open '" << filename << "'.");
				sqlite3_exec(database, "COMMIT;", 0, 0, 0);
				sqlite3_finalize(qry2);
				sqlite3_finalize(stmt1);
				return false;
			}
			break;
		}

		BettiTable b;

		while (b.read_next_table(&kFile))
		{
			if (sqlite3_step(qry2) != SQLITE_ROW)
			{
				FAIL("Adding Betti data", "There are more tables in the files than graphs satisfying condition '" << *query_condition << "' in the database.");
				kFile.close();
				sqlite3_exec(database, "COMMIT;", 0, 0, 0);
				sqlite3_finalize(qry2);
				sqlite3_finalize(stmt1);
				return false;
			}

			sqlite3_bind_text(stmt1, 1, b.convert_to_line().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt1, 2, b.get_projective_dimension());
			sqlite3_bind_int(stmt1, 3, b.get_regularity());
			sqlite3_bind_text(stmt1, 4, b.get_extremal_betti_numbers_as_string().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt1, 5, sqlite3_column_int(qry2, 0));

			sqlite3_step(stmt1);

			sqlite3_clear_bindings(stmt1);
			sqlite3_reset(stmt1);

			count++;

			if (count == 10000)
			{
				PROGRESS(2, count << " graphs updated");
				count = 0;
			}
		}

		kFile.close();
	}

	if (count != 0)
		PROGRESS(2, count << " graphs updated");
	
	sqlite3_exec(database, "COMMIT;", 0, 0, 0);
	sqlite3_finalize(qry2);
	sqlite3_finalize(stmt1);

	return true;
}


/**
* reads all degrees of h-polynomials from the files determined by the given name, datetime and index, updates all graphs satisfying query_condition
**/
bool DatabaseInterface::insert_hpoldeg_data(std::string * name, std::string * query_condition, std::string * datetime, unsigned index) {
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + *name + "Hpoldeg INT;").c_str(), 0, 0, 0);

	sqlite3_stmt * qry2;
	if (query_condition->empty())
	{
		if (sqlite3_prepare_v2(database, "SELECT graphID FROM Graphs", -1, &qry2, 0) != SQLITE_OK)
		{
			SQL_ERROR("SELECT graphID FROM Graphs");
			FAIL("Adding hpoldeg data", "");
			sqlite3_finalize(qry2);
			return false;
		}
	}
	else
	{
		if (sqlite3_prepare_v2(database, ("SELECT graphID FROM Graphs WHERE " + *query_condition).c_str(), -1, &qry2, 0) != SQLITE_OK)
		{
			SQL_ERROR("SELECT graphID FROM Graphs" << *query_condition);
			FAIL("Adding hpoldeg data", "");
			sqlite3_finalize(qry2);
			return false;
		}
	}

	std::string statement = "UPDATE Graphs SET " + *name + "Hpoldeg = ? WHERE graphID == ?;";
	sqlite3_stmt * stmt1;

	if (sqlite3_prepare_v2(database, statement.c_str(), -1, &stmt1, 0) != SQLITE_OK)
	{
		SQL_ERROR(statement);
		FAIL("Adding hpoldeg data", "");
		sqlite3_finalize(qry2);
		sqlite3_finalize(stmt1);
		return false;
	}

	sqlite3_exec(database, "BEGIN TRANSACTION;", 0, 0, 0);
	unsigned count = 0;
	for (unsigned k = 0; true; k++)
	{
		std::string filename = std::to_string(index) + "_" + *datetime + "_" + *name + "_" + std::to_string(k) + ".result";
		std::ifstream kFile(filename);

		if (!kFile.is_open())
		{
			if (k == 0)
			{
				FAIL("Adding hpoldeg data", "Unable to open '" << filename << "'.");
				sqlite3_exec(database, "COMMIT;", 0, 0, 0);
				sqlite3_finalize(qry2);
				sqlite3_finalize(stmt1);
				return false;
			}
			break;
		}

		std::string line;

		while (getline(kFile, line))
		{
			if (line.empty())
				continue;

			int hpoldeg = 0;

			for (unsigned i = line.at(0) == '-' ? 1 : 0; i < line.length(); i++)
			{
				if (line.at(i) < '0'
					|| line.at(i) > '9')
				{
					PARSE_ERROR("'" << line << "' is not a number.");
					FAIL("Adding hpoldeg data", "");
					kFile.close();
					sqlite3_exec(database, "COMMIT;", 0, 0, 0);
					sqlite3_finalize(qry2);
					sqlite3_finalize(stmt1);
					return false;
				}

				hpoldeg *= 10;
				hpoldeg += line.at(i) - '0';
			}

			if (line.at(0) == '-')
				hpoldeg *= -1;

			if (sqlite3_step(qry2) != SQLITE_ROW)
			{
				FAIL("Adding hpoldeg data", "There are more numbers in the files than graphs satisfying condition '" << *query_condition << "' in the database.");
				kFile.close();
				sqlite3_exec(database, "COMMIT;", 0, 0, 0);
				sqlite3_finalize(qry2);
				sqlite3_finalize(stmt1);
				return false;
			}

			sqlite3_bind_int(stmt1, 1, hpoldeg);
			sqlite3_bind_int(stmt1, 2, sqlite3_column_int(qry2, 0));

			sqlite3_step(stmt1);

			sqlite3_clear_bindings(stmt1);
			sqlite3_reset(stmt1);

			count++;

			if (count == 10000)
			{
				PROGRESS(2, count << " graphs updated");
				count = 0;
			}
		}

		kFile.close();
	}

	if (count != 0)
		PROGRESS(2, count << " graphs updated");

	sqlite3_exec(database, "COMMIT;", 0, 0, 0);
	sqlite3_finalize(qry2);
	sqlite3_finalize(stmt1);

	return true;
}
