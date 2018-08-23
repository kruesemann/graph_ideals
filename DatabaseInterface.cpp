#include "DatabaseInterface.h"


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
* returns x to the power of n
**/
inline unsigned nth_power(unsigned x, unsigned n) {
	if (x == 0)
		return 0;

	unsigned power = 1;

	for (unsigned i = 0; i < n; i++)
		power *= x;

	return power;
}


/**
 * returns the appropriate coordinates for vertex to print the graph of given order on a circle
**/
std::pair<int, int> get_coordinate(unsigned vertex, unsigned order) {
	int first = (int)100 * cos(2 * (vertex - 1) * PI / order);
	int second = (int)100 * sin(2 * (vertex - 1) * PI / order);

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
 * outputs the current view to the terminal
**/
void DatabaseInterface::show_view(int limit) {
	if (number_columns != 0)
	{
		for (unsigned j = 0; j < number_columns - 1; j++)
			std::cout << view_columns[j] << std::string(column_widths[j] - view_columns[j].length(), ' ');
		std::cout << view_columns[number_columns - 1] << "\n";

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
* outputs the current view to given file
**/
void DatabaseInterface::save_view(std::ofstream * file) {
	if (number_columns != 0)
	{
		for (unsigned j = 0; j < number_columns - 1; j++)
			*file << view_columns[j] << std::string(column_widths[j] - view_columns[j].length(), ' ');
		*file << view_columns[number_columns - 1] << "\n";

		for (unsigned i = 0; i < number_rows; i++)
		{
			for (unsigned j = 0; j < number_columns - 1; j++)
				*file << view_contents[i][j] << std::string(column_widths[j] - view_contents[i][j].length(), ' ');
			*file << view_contents[i][number_columns - 1] << "\n";
		}
	}
}


/**
* outputs the current view to the terminal in rich format
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

			std::cout << std::string(overall_width, '-') << "\n" << columns << "\n";

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
* outputs the current view to given file in rich format
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
			*file << std::string(overall_width, '-') << "\n" << columns << "\n";

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
 * outputs all graphs of the current view to given file in LaTeX-TikZ format (also outputs necessary LaTeX-package includes)
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
				std::cout << "Saving view failed. Visualisation does not support multiple graphs per row." << std::endl;
				return;
			}
		}
		else if (view_columns[i] == "graphOrder")
		{
			if (graphOrder_index == -1)
				graphOrder_index = i;
			else
			{
				std::cout << "Saving view failed. Visualisation does not support multiple graphs per row." << std::endl;
				return;
			}
		}
		else if (view_columns[i] == "edges")
		{
			if (edges_index == -1)
				edges_index = i;
			else
			{
				std::cout << "Saving view failed. Visualisation does not support multiple graphs per row." << std::endl;
				return;
			}
		}
	}

	if (graphOrder_index == -1
		|| edges_index == -1)
	{
		std::cout << "Saving view failed. View does not include both graphOrder and edges. These are necessary to visualize the graphs." << std::endl;
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
* outputs all graphs in the current view to given file in g6 format
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
		std::cout << "Saving view failed. View does not include both graphOrder and edges. These are necessary to visualize the graphs." << std::endl;
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
 * generates Macaulay2 scripts to compute the Betti tables for the factor ring of the specified ideal type of all graphs
**/
void DatabaseInterface::generate_m2_scripts(std::string * ideal_type, unsigned batch_size, std::vector<std::string> * required_packages) {
	sqlite3_stmt * qry;

	if (sqlite3_prepare_v2(database, "SELECT edges FROM Graphs WHERE graphOrder == ?", -1, &qry, 0) != SQLITE_OK)
	{
		std::cout << "Generating M2 scripts failed. SQL error: invalid query." << std::endl;
		sqlite3_finalize(qry);
		return;
	}

	for (unsigned n = 1; true; n++)
	{
		sqlite3_bind_int(qry, 1, n);

		std::string vertices = "";
		for (unsigned v = 1; v <= n; v++)
			vertices += std::to_string(v) + ",";
		vertices.pop_back();

		unsigned k;
		unsigned i;

		for (k = 0; true; k++)
		{
			std::string script = "G = {\n";
			std::string edges;
			int empty_index = -1;

			for (i = 0; i < batch_size && sqlite3_step(qry) == SQLITE_ROW; i++)
			{
				edges = (char *)sqlite3_column_text(qry, 0) ? (char *)sqlite3_column_text(qry, 0) : "ERROR";
				script += edges + ",\n";
				if (edges == "{}")
					empty_index = i;
			}

			if (i != 0)
			{
				script.pop_back();
				script.pop_back();

				script += "\n}\n\nneedsPackage(\"Graphs\")\n";

				for (unsigned j = 0; j < required_packages->size(); j++)
					script += "needsPackage(\"" + required_packages->at(j) + "\")\n";

				std::ofstream iFile;
				size_t cut_index = ideal_type->find_first_of(' ');

				if (cut_index < ideal_type->length() - 1)
				{
					std::string temp1 = ideal_type->substr(0, cut_index);
					std::string temp2 = ideal_type->substr(cut_index + 1, std::string::npos);

					iFile.open(std::to_string(n) + "-graphs_" + temp1 + "_" + temp2 + "_" + std::to_string(k) + ".m2", std::ios::trunc);

					if (!iFile.is_open())
					{
						std::cout << "Generating M2 scripts failed. Could not write to file." << std::endl;
						sqlite3_finalize(qry);
						return;
					}

					if (empty_index != -1)
						script += "\nfor i from 0 to " + std::to_string(empty_index - 1) + " do B_i=betti res " + *ideal_type + "(graph({" + vertices + "},G_i))\n" \
						"B_" + std::to_string(empty_index) + "=betti res " + temp2 + "(graph({" + vertices + "},G_" + std::to_string(empty_index) + "))\n" \
						"for i from " + std::to_string(empty_index + 1) + " to " + std::to_string(i - 1) + " do B_i=betti res " + *ideal_type + "(graph({" + vertices + "},G_i))\n\n" \
						"F:=openOut(\"" + std::to_string(n) + "-betti_" + temp1 + "_" + temp2 + "_" + std::to_string(k) + ".bt\")\n";
					else
						script += "\nfor i from 0 to " + std::to_string(i - 1) + " do B_i=betti res " + *ideal_type + "(graph({" + vertices + "},G_i))\n\n" \
						"F:=openOut(\"" + std::to_string(n) + "-betti_" + temp1 + "_" + temp2 + "_" + std::to_string(k) + ".bt\")\n";
				}
				else
				{
					iFile.open(std::to_string(n) + "-graphs_" + *ideal_type + "_" + std::to_string(k) + ".m2", std::ios::trunc);

					if (!iFile.is_open())
					{
						std::cout << "Generating M2 scripts failed. Could not write to file." << std::endl;
						sqlite3_finalize(qry);
						return;
					}

					script += "\nfor i from 0 to " + std::to_string(i - 1) + " do B_i=betti res " + *ideal_type + "(graph({" + vertices + "},G_i))\n\n" \
						"F:=openOut(\"" + std::to_string(n) + "-betti_" + *ideal_type + "_" + std::to_string(k) + ".bt\")\n";
				}

				script += "for i from 0 to " + std::to_string(i - 1) + " do F << B_i << endl << endl\nclose F";

				iFile << script;

				iFile.close();

				std::cout << "   generated " << n << "-graphs_" << *ideal_type << "_" << k << ".m2\n";
			}

			if (i < batch_size)
				break;
		}

		sqlite3_clear_bindings(qry);
		sqlite3_reset(qry);

		if (k == 0
			&& i == 0)
			break;
	}

	sqlite3_finalize(qry);
}


/**
* generates Macaulay2 scripts to compute the Betti tables for the factor ring of the initial ideal of the specified ideal type of all closed graphs each labeled wrt to one of their closed orderings
**/
void DatabaseInterface::generate_closed_labeling_m2_scripts(std::string * ideal_type, unsigned batch_size, std::vector<std::string> * required_packages) {
	sqlite3_stmt * qry;

	if (sqlite3_prepare_v2(database, "SELECT edges FROM Graphs WHERE graphOrder == ? AND type LIKE '%closed%'", -1, &qry, 0) != SQLITE_OK)
	{
		std::cout << "Generating M2 scripts failed. SQL error: invalid query." << std::endl;
		sqlite3_finalize(qry);
		return;
	}

	for (unsigned n = 1; true; n++)
	{
		sqlite3_bind_int(qry, 1, n);

		std::string vertices = "";
		for (unsigned v = 1; v <= n; v++)
			vertices += std::to_string(v) + ",";
		vertices.pop_back();

		unsigned k;
		unsigned i;

		for (k = 0; true; k++)
		{
			std::string script = "G = {\n";
			std::string edges;
			int empty_index = -1;

			for (i = 0; i < batch_size && sqlite3_step(qry) == SQLITE_ROW; i++)
			{
				edges = (char *)sqlite3_column_text(qry, 0) ? (char *)sqlite3_column_text(qry, 0) : "ERROR";
				Graph g(n, &edges);
				std::pair<unsigned *, unsigned *> closed_ordering = g.gen_closed_ordering();
				script += g.convert_to_string_wrt_ordering(closed_ordering.second) + ",\n";
				delete[] closed_ordering.first;
				delete[] closed_ordering.second;
				if (edges == "{}")
					empty_index = i;
			}

			if (i != 0)
			{
				script.pop_back();
				script.pop_back();

				script += "\n}\n\nneedsPackage(\"Graphs\")\n";

				for (unsigned j = 0; j < required_packages->size(); j++)
					script += "needsPackage(\"" + required_packages->at(j) + "\")\n";

				size_t cut_index = ideal_type->find_first_of(' ');

				if (cut_index < ideal_type->length() - 1)
				{
					
					std::cout << "Generating M2 scripts failed. Ideal type cannot be a combination two types." << std::endl;
					sqlite3_finalize(qry);
					return;
				}
				
				std::ofstream iFile(std::to_string(n) + "-graphs_closed_" + *ideal_type + "_" + std::to_string(k) + ".m2", std::ios::trunc);

				if (!iFile.is_open())
				{
					std::cout << "Generating M2 scripts failed. Could not write to file." << std::endl;
					sqlite3_finalize(qry);
					return;
				}

				if (empty_index != -1)
					script += "\nfor i from 0 to " + std::to_string(empty_index - 1) + " do B_i=betti res monomialIdeal " + *ideal_type + "(graph({" + vertices + "},G_i))\n" \
					"B_" + std::to_string(empty_index) + "=betti res " + *ideal_type + "(graph({" + vertices + "},G_" + std::to_string(empty_index) + "))\n" \
					"for i from " + std::to_string(empty_index + 1) + " to " + std::to_string(i - 1) + " do B_i=betti res monomialIdeal " + *ideal_type + "(graph({" + vertices + "},G_i))\n\n" \
					"F:=openOut(\"" + std::to_string(n) + "-betti_closed_" + *ideal_type + "_" + std::to_string(k) + ".bt\")\n";
				else
					script += "\nfor i from 0 to " + std::to_string(i - 1) + " do B_i=betti res monomialIdeal " + *ideal_type + "(graph({" + vertices + "},G_i))\n\n" \
					"F:=openOut(\"" + std::to_string(n) + "-betti_closed_" + *ideal_type + "_" + std::to_string(k) + ".bt\")\n";

				script += "for i from 0 to " + std::to_string(i - 1) + " do F << B_i << endl << endl\nclose F";

				iFile << script;

				iFile.close();

				std::cout << "   generated " << n << "-graphs_closed_" << *ideal_type << "_" << k << ".m2\n";
			}

			if (i < batch_size)
				break;
		}

		sqlite3_clear_bindings(qry);
		sqlite3_reset(qry);

		if (k == 0
			&& i == 0)
			break;
	}

	sqlite3_finalize(qry);
}


/**
 * generates two files: one for the graphIDs of the graphs in the database that are cones over other graphs, one for the base graphs of these cones
**/
void DatabaseInterface::generate_cone_lists() {
	std::ofstream coneIDsFile("cone_IDs.txt", std::ios::trunc);

	if (!coneIDsFile.is_open())
	{
		std::cout << "Generating cone files failed. Could not write to file." << std::endl;
		return;
	}

	std::ofstream coneBasesFile("cone_bases.g6", std::ios::trunc);

	if (!coneBasesFile.is_open())
	{
		std::cout << "Generating cone files failed. Could not write to file." << std::endl;
		coneIDsFile.close();
		return;
	}

	sqlite3_stmt * qry;

	if (sqlite3_prepare_v2(database, "SELECT graphID,graphOrder,edges FROM Graphs;", -1, &qry, 0) != SQLITE_OK)
	{
		std::cout << "Generating cone files failed. SQL error: invalid query." << std::endl;
		coneIDsFile.close();
		coneBasesFile.close();
		sqlite3_finalize(qry);
		return;
	}

	while (sqlite3_step(qry) == SQLITE_ROW)
	{
		unsigned graphID = sqlite3_column_int(qry, 0);
		unsigned graphOrder = sqlite3_column_int(qry, 1);
		std::string edges = (char *)sqlite3_column_text(qry, 2) ? (char *)sqlite3_column_text(qry, 2) : "ERROR";

		Graph g(graphOrder, &edges);
		Graph base = g.get_base_of_cone();

		if (base.get_order() != 0)
		{
			coneIDsFile << graphID << "\n";
			coneBasesFile << base.convert_to_g6_format() << "\n";
		}
	}

	coneIDsFile.close();
	coneBasesFile.close();
	sqlite3_finalize(qry);
}


/**
 * compares the regularities of the cones in cone_IDs.txt and of their now canonically labeled base graphs in file of given name
**/
void DatabaseInterface::compare_cone_regularities(std::string * filename) {
	std::ifstream coneIDsFile("cone_IDs.txt");

	if (!coneIDsFile.is_open())
	{
		std::cout << "Comparing cone regularities failed. Could not open cone_IDs.txt." << std::endl;
		return;
	}

	std::ifstream coneBasesFile(filename->c_str());

	if (!coneBasesFile.is_open())
	{
		std::cout << "Comparing cone regularities failed. Could not open " << filename << "." << std::endl;
		coneIDsFile.close();
		return;
	}

	sqlite3_stmt * id_qry;

	if (sqlite3_prepare_v2(database, "SELECT graphOrder,edges,beiReg FROM Graphs WHERE graphID == ?", -1, &id_qry, 0) != SQLITE_OK)
	{
		std::cout << "Comparing cone regularities failed. SQL error: invalid query." << std::endl;
		coneIDsFile.close();
		coneBasesFile.close();
		sqlite3_finalize(id_qry);
		return;
	}

	sqlite3_stmt * base_qry;

	if (sqlite3_prepare_v2(database, "SELECT graphOrder,edges,beiReg FROM Graphs WHERE graphOrder == ? AND edges == ?", -1, &base_qry, 0) != SQLITE_OK)
	{
		std::cout << "Comparing cone regularities failed. SQL error: invalid query." << std::endl;
		coneIDsFile.close();
		coneBasesFile.close();
		sqlite3_finalize(id_qry);
		sqlite3_finalize(base_qry);
		return;
	}

	while (true)
	{
		std::string id_line;
		Graph base;

		if (!getline(coneIDsFile, id_line)
			|| !base.read_next_g6_format(&coneBasesFile))
			break;

		std::stringstream id_stream(id_line);
		unsigned id;
		if (id_stream >> id)
			sqlite3_bind_int(id_qry, 1, id);
		else
		{
			std::cout << "Comparing cone regularities failed. Parse error." << std::endl;
			coneIDsFile.close();
			coneBasesFile.close();
			sqlite3_finalize(id_qry);
			sqlite3_finalize(base_qry);
			return;
		}

		if (sqlite3_step(id_qry) != SQLITE_ROW)
		{
			std::cout << "Comparing cone regularities failed. Could not find graph with graphID " << id << "." << std::endl;
			coneIDsFile.close();
			coneBasesFile.close();
			sqlite3_finalize(id_qry);
			sqlite3_finalize(base_qry);
			return;
		}

		sqlite3_bind_int(base_qry, 1, base.get_order());
		sqlite3_bind_text(base_qry, 2, base.convert_to_string().c_str(), -1, SQLITE_TRANSIENT);

		if (sqlite3_step(base_qry) == SQLITE_ROW
			&& base.get_order() != 1)
		{
			if (sqlite3_column_int(id_qry, 2) != sqlite3_column_int(base_qry, 2))
			{
				std::cout << "Found non-matching pair: " << sqlite3_column_text(id_qry, 1) << "  " << sqlite3_column_int(id_qry, 0) << " and " << sqlite3_column_text(base_qry, 1) << "  " << sqlite3_column_int(base_qry, 0) << "." << std::endl;
				coneIDsFile.close();
				coneBasesFile.close();
				sqlite3_finalize(id_qry);
				sqlite3_finalize(base_qry);
				return;
			}
		}

		sqlite3_clear_bindings(id_qry);
		sqlite3_reset(id_qry);
		sqlite3_clear_bindings(base_qry);
		sqlite3_reset(base_qry);
	}
	
	std::cout << "Found no non-matching pairs." << std::endl;
	coneIDsFile.close();
	coneBasesFile.close();
	sqlite3_finalize(id_qry);
	sqlite3_finalize(base_qry);
	return;
}


/**
 * outputs the status table to the terminal
**/
void DatabaseInterface::show_status() {
	sqlite3_stmt * stmt;
	if (sqlite3_prepare_v2(database, "SELECT name,specification,type,checked,help_text FROM Status;", -1, &stmt, 0) != SQLITE_OK)
	{
		std::cout << "Showing status failed. SQL error: Could not find status table." << std::endl;
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
		widths.push_back(column.length() + 3 < max_column_width ? column.length() + 3 : max_column_width + 3);
	}

	std::vector<std::vector<std::string>> contents;
	unsigned rowc = 0;

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		std::vector<std::string> row;
		for (unsigned i = 0; i < colc; i++)
		{
			std::string content;
			if (i == 3)
			{
				int status = sqlite3_column_int(stmt, i) ? sqlite3_column_int(stmt, i) : -1;
				if (status == -1)
					content = "-";
				else
				{
					content = "";
					unsigned power = 1;
					for (unsigned n = 1; status != 0; n++)
					{
						if ((status % (2 * power)) / power == 1)
						{
							content += std::to_string(n) + ",";
							status -= power;
						}
						power *= 2;
					}
					
					if (!content.empty())
						content.pop_back();
					else
						content = "-";
				}
			}
			else
				content = (char *)sqlite3_column_text(stmt, i) ? (char *)sqlite3_column_text(stmt, i) : "-";

			row.push_back(content);

			if (widths[i] < content.length() + 3)
				widths[i] = content.length() < max_column_width ? content.length() + 3 : max_column_width + 3;
		}
		contents.push_back(row);
		rowc++;
	}

	sqlite3_finalize(stmt);

	unsigned last_whitespace = 0;
	for (unsigned j = 0; j < colc; j++)
	{
		std::cout << std::string(last_whitespace, ' ') << columns[j];
		last_whitespace = widths[j] - columns[j].length();
	}
	std::cout << "\n";

	for (unsigned i = 0; i < rowc; i++)
	{
		std::vector<std::string> overflow;
		bool wrap = false;
		last_whitespace = 0;

		for (unsigned j = 0; j < colc; j++)
		{
			std::string content = contents[i][j];
			if (content.length() <= max_column_width)
			{
				std::cout << std::string(last_whitespace, ' ') << content;
				last_whitespace = widths[j] - content.length();
				overflow.push_back("");
			}
			else
			{
				std::cout << std::string(last_whitespace, ' ') << content.substr(0, max_column_width);
				last_whitespace = 3;
				overflow.push_back(content.substr(max_column_width, std::string::npos));
				wrap = true;
			}
		}
		std::cout << "\n";

		while (wrap)
		{
			wrap = false;
			last_whitespace = 0;

			for (unsigned j = 0; j < colc; j++)
			{
				std::string content = overflow[j];
				if (content.length() <= max_column_width)
				{
					std::cout << std::string(last_whitespace, ' ') << content;
					last_whitespace = widths[j] - content.length();
					overflow[j] = "";
				}
				else
				{
					std::cout << std::string(last_whitespace, ' ') << content.substr(0, max_column_width);
					last_whitespace = 3;
					overflow[j] = content.substr(max_column_width, std::string::npos);
					wrap = true;
				}
			}
			std::cout << "\n";
		}
	}
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
			std::cout << "SQL error: invalid query." << std::endl;
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
		std::cout << "SQL query failed. " << error << ". View reset." << std::endl;
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
		std::cout << "SQL error: " << errMsg << "." << std::endl;
		sqlite3_free(errMsg);
		return false;
	}

	return true;
}


/**
 * creates the status table in database for ckecking the build status
**/
bool DatabaseInterface::create_status_table() {
	std::string statement = "CREATE TABLE Status(" \
		"columnID INTEGER PRIMARY KEY," \
		"name TEXT NOT NULL," \
		"specification TEXT," \
		"type TEXT NOT NULL,"
		"checked INT," \
		"help_text TEXT" \
		");";

	if (sqlite3_exec(database, statement.c_str(), 0, 0, 0) != SQLITE_OK)
		return false;

	statement = "INSERT INTO Status(name,specification,type,checked,help_text) VALUES" \
		"('graphID',NULL,'INT',NULL,'unique graph number in sequence of insertion')," \
		"('graphOrder',NULL,'INT',NULL,'number of vertices')," \
		"('graphSize',NULL,'INT',NULL,'number of edges')," \
		"('edges',NULL,'TEXT',NULL,'list of edges')," \
		"('cliqueNumber',NULL,'INT',NULL,'cardinality of the largest clique')," \
		"('maxCliqueNumber',NULL,'INT',NULL,'number of inclusion-maximal cliques')," \
		"('type','connected','TEXT',0,'a graph for which there is a path between every pair of vertices')," \
		"('type','cograph','TEXT',0,'a graph which does not contain a path of length 3')," \
		"('type','euler','TEXT',0,'a graph for which all vertices are of even degree')," \
		"('type','chordal','TEXT',0,'a graph for which exists a perfect elimination ordering')," \
		"('type','claw-free','TEXT',0,'a graph which does not contain a claw, i.e. a complete 1-3-bipartite induced subgraph')," \
		"('type','closed','TEXT',0,'a graph for which exists an ordering with the following property: {a,b}, {i,j} in E(G) where a<b, i<j  =>  {b,j} in E(G) if a=i and {a,i} in E(G) if b=j')," \
		"('[ideal]Bettis',NULL,'TEXT',NULL,'Betti table of the free resolution of the factor ring of [ideal]')," \
		"('[ideal]PD',NULL,'INT',NULL,'projective dimension of the factor ring of [ideal]')," \
		"('[ideal]Reg',NULL,'INT',NULL,'Castelnuovo-Mumford regularity of the free resolution')," \
		"('[ideal]Schenzel',NULL,'INT',NULL,'column number (starting with 1) of the rightmost non-zero entry of the second row of the Betti table; 1 if there is no second row')," \
		"('[ideal]Extremals',NULL,'TEXT',NULL,'extremal Betti numbers of the Betti table');";
	if (!execute_SQL_statement(&statement))
		return false;

	return true;
}


/**
 * creates graphs table in database
**/
bool DatabaseInterface::create_graphs_table() {
	std::string statement = "CREATE TABLE IF NOT EXISTS Graphs("  \
							"graphID INTEGER PRIMARY KEY," \
							"graphOrder INT NOT NULL," \
							"graphSize INT NOT NULL," \
							"edges TEXT NOT NULL," \
							"cliqueNumber INT," \
							"maxCliqueNumber INT," \
							"type TEXT" \
							");";

	return execute_SQL_statement(&statement);
}


/**
 * batch inserts graphs from given file
**/
void DatabaseInterface::insert_graphs(std::ifstream * file, FORMAT format) {
	if (format == FORMAT::NONE)
		throw "noFormat";

	sqlite3_exec(database, "BEGIN TRANSACTION;", 0, 0, 0);

	bool (Graph::*read_function)(std::ifstream *) = &Graph::read_next_g6_format;
	
	if (format == FORMAT::LIST)
		read_function = &Graph::read_next_list_format;
	
	while (true)
	{
		unsigned i;
		Graph g;
		std::string statement = "INSERT INTO Graphs (graphOrder,graphSize,edges,cliqueNumber,maxCliqueNumber) VALUES ";

		for (i = 0; i < 10000 && (g.*read_function)(file); i++)
		{
			std::pair<unsigned, unsigned> clique_numbers = g.get_clique_numbers();
			statement += "(" + std::to_string(g.get_order()) + "," + std::to_string(g.get_size()) + ",'" + g.convert_to_string() + "'," + std::to_string(clique_numbers.first) + "," + std::to_string(clique_numbers.second) + "),";//TEST
		}

		statement.pop_back();

		execute_SQL_statement(&statement);

		std::cout << "   " << i << " graphs inserted\n";

		if (i < 10000)
			break;
	}

	sqlite3_exec(database, "COMMIT;", 0, 0, 0);
}


/**
 * updates the type of all graphs that satisfy graph_test and query_condition
**/
bool DatabaseInterface::update_type(bool(Graph::*graph_test)(), const char * type, const char * query_condition) {
	std::string query = "SELECT graphID,graphorder,edges,type FROM Graphs WHERE (type IS NULL OR type NOT LIKE '%" + std::string(type) + "%')";
	if (query_condition)
		query += " AND " + std::string(query_condition);

	sqlite3_stmt * qry;

	if (sqlite3_prepare_v2(database, query.c_str(), -1, &qry, 0) != SQLITE_OK)
	{
		std::cout << "Update failed. SQL error: '" << query << "' is an invalid query." << std::endl;
		sqlite3_finalize(qry);
		return false;
	}

	sqlite3_stmt * stmt;

	if (sqlite3_prepare_v2(database, "UPDATE Graphs SET type = ? WHERE graphID == ?", -1, &stmt, 0) != SQLITE_OK)
	{
		std::cout << "Update failed. SQL error: '" << stmt << "' is an invalid statement." << std::endl;
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
			std::cout << "      " << i << " graphs tested\n";
			i = 1;
		}
	}
	sqlite3_exec(database, "COMMIT;", 0, 0, 0);

	sqlite3_finalize(qry);
	sqlite3_finalize(stmt);

	if (i > 1)
		std::cout << "      " << i - 1 << " graphs tested\n";
	else
	{
		std::cout << "   Unable to find any graphs satisfying the condition of the query: " << query << "." << std::endl;
		return false;
	}

	return true;
}


/**
 * reads all Betti tables from the files determined by the given order and ideal_type, updates the graphs table and all graphs of given order
**/
bool DatabaseInterface::add_betti_data(std::string * idealname, const char * query_condition) {
	size_t cut_index = idealname->find_first_of(' ');
	std::string ideal;
	if (cut_index < idealname->length() - 1)
		ideal = idealname->substr(0, cut_index) + idealname->substr(cut_index + 1, std::string::npos);
	else
		ideal = *idealname;

	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + ideal + "Bettis TEXT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + ideal + "PD INT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + ideal + "Reg INT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + ideal + "Schenzel INT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + ideal + "Extremals TEXT;").c_str(), 0, 0, 0);

	sqlite3_stmt * qry;
	if (query_condition)
	{
		if (sqlite3_prepare_v2(database, ("SELECT graphID FROM Graphs WHERE " + std::string(query_condition) + ";").c_str(), -1, &qry, 0) != SQLITE_OK)
		{
			std::cout << "Adding Betti data failed. SQL error: '" << qry << "' is an invalid query." << std::endl;
			sqlite3_finalize(qry);
			return false;
		}
	}
	else
	{
		if (sqlite3_prepare_v2(database, "SELECT graphID FROM Graphs;", -1, &qry, 0) != SQLITE_OK)
		{
			std::cout << "Adding Betti data failed. SQL error: '" << qry << "' is an invalid query." << std::endl;
			sqlite3_finalize(qry);
			return false;
		}
	}
	

	std::string statement = "UPDATE Graphs SET " + ideal + "Bettis = ?, " + ideal + "PD = ?, " + ideal + "Reg = ?, " + ideal + "Schenzel = ?, " + ideal + "Extremals = ? WHERE graphID == ?;";
	sqlite3_stmt * stmt;

	if (sqlite3_prepare_v2(database, statement.c_str(), -1, &stmt, 0) != SQLITE_OK)
	{
		std::cout << "Adding Betti data failed. SQL error: '" << stmt << "' is an invalid statement." << std::endl;
		sqlite3_finalize(qry);
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_exec(database, "BEGIN TRANSACTION;", 0, 0, 0);
	unsigned count = 0;
	for (unsigned k = 0; true; k++)
	{
		std::ifstream kFile("betti_" + ideal + "_" + std::to_string(k) + ".bt");

		if (!kFile.is_open())
		{
			if (k == 0)
			{
				std::cout << "Adding Betti data failed. Unable to open 'betti_" << ideal << "_0.bt'." << std::endl;
				sqlite3_exec(database, "COMMIT;", 0, 0, 0);
				sqlite3_finalize(qry);
				sqlite3_finalize(stmt);
				return false;
			}
			break;
		}

		BettiTable b;

		for (unsigned i = 1; b.read_next_table(&kFile); i++)
		{
			if (sqlite3_step(qry) != SQLITE_ROW)
			{
				std::cout << "Adding Betti data failed. There are more tables in the files than graphs of order satisfying condition '" << query_condition << "' in the database." << std::endl;
				sqlite3_exec(database, "COMMIT;", 0, 0, 0);
				sqlite3_finalize(qry);
				sqlite3_finalize(stmt);
				return false;
			}

			sqlite3_bind_text(stmt, 1, b.convert_to_line().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 2, b.get_projective_dimension());
			sqlite3_bind_int(stmt, 3, b.get_regularity());
			sqlite3_bind_int(stmt, 4, b.get_schenzel_number());
			sqlite3_bind_text(stmt, 5, b.get_extremal_betti_numbers_as_string().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 6, sqlite3_column_int(qry, 0));

			sqlite3_step(stmt);

			sqlite3_clear_bindings(stmt);
			sqlite3_reset(stmt);

			count++;

			if (count == 10000)
			{
				std::cout << "      " << count << " graphs updated.\n";
				count = 0;
			}
		}
	}
	if (count != 0)
		std::cout << "      " << count << " graphs updated.\n";
	sqlite3_exec(database, "COMMIT;", 0, 0, 0);

	sqlite3_finalize(qry);
	sqlite3_finalize(stmt);

	return true;
}


/**
* reads all Betti tables from the files determined by the given order and ideal_type, updates the graphs table and all closed graphs of given order
**/
bool DatabaseInterface::add_closed_labeling_betti_data(unsigned order, std::string * name, std::string * ideal_type) {
	size_t cut_index = ideal_type->find_first_of(' ');
	std::string filename;
	if (cut_index < ideal_type->length() - 1)
	{
		std::cout << "Generating M2 scripts failed. Ideal type cannot be a combination two types." << std::endl;
		return false;
	}
	else
		filename = "closed_" + *ideal_type;

	size_t cut_index_2 = name->find_first_of(' ');
	std::string ideal;
	if (cut_index_2 < name->length() - 1)
		ideal = name->substr(0, cut_index_2) + name->substr(cut_index_2 + 1, std::string::npos);
	else
		ideal = *name;

	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + ideal + "Bettis TEXT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + ideal + "PD INT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + ideal + "Reg INT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + ideal + "Schenzel INT;").c_str(), 0, 0, 0);
	sqlite3_exec(database, (std::string("ALTER TABLE Graphs ADD ") + ideal + "Extremals TEXT;").c_str(), 0, 0, 0);

	sqlite3_stmt * qry;
	if (sqlite3_prepare_v2(database, ("SELECT graphID FROM Graphs WHERE graphOrder == " + std::to_string(order) + " AND type LIKE '%closed%';").c_str(), -1, &qry, 0) != SQLITE_OK)
	{
		std::cout << "Adding Betti data failed. SQL error: invalid query." << std::endl;
		sqlite3_finalize(qry);
		return false;
	}

	std::string statement = "UPDATE Graphs SET " + ideal + "Bettis = ?, " + ideal + "PD = ?, " + ideal + "Reg = ?, " + ideal + "Schenzel = ?, " + ideal + "Extremals = ? WHERE graphID == ?;";
	sqlite3_stmt * stmt;

	if (sqlite3_prepare_v2(database, statement.c_str(), -1, &stmt, 0) != SQLITE_OK)
	{
		std::cout << "Adding Betti data failed. SQL error: invalid statement." << std::endl;
		sqlite3_finalize(qry);
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_exec(database, "BEGIN TRANSACTION;", 0, 0, 0);
	unsigned count = 0;
	for (unsigned k = 0; true; k++)
	{
		std::ifstream kFile(std::to_string(order) + "-betti_" + filename + "_" + std::to_string(k) + ".bt");

		if (!kFile.is_open())
		{
			if (k == 0)
			{
				std::cout << "Adding Betti data failed. Could not open " << order << "-betti_" << filename << "_0.bt." << std::endl;
				sqlite3_exec(database, "COMMIT;", 0, 0, 0);
				sqlite3_finalize(qry);
				sqlite3_finalize(stmt);
				return false;
			}
			break;
		}

		BettiTable b;

		for (unsigned i = 1; b.read_next_table(&kFile); i++)
		{
			if (sqlite3_step(qry) != SQLITE_ROW)
			{
				std::cout << "Adding Betti data failed. There are more tables in the file than graphs of order " << order << " in the database." << std::endl;
				sqlite3_exec(database, "COMMIT;", 0, 0, 0);
				sqlite3_finalize(qry);
				sqlite3_finalize(stmt);
				return false;
			}

			sqlite3_bind_text(stmt, 1, b.convert_to_line().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 2, b.get_projective_dimension());
			sqlite3_bind_int(stmt, 3, b.get_regularity());
			sqlite3_bind_int(stmt, 4, b.get_schenzel_number());
			sqlite3_bind_text(stmt, 5, b.get_extremal_betti_numbers_as_string().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 6, sqlite3_column_int(qry, 0));

			sqlite3_step(stmt);

			sqlite3_clear_bindings(stmt);
			sqlite3_reset(stmt);

			count++;

			if (count == 10000)
			{
				std::cout << "   " << count << " graphs updated.\n";
				count = 0;
			}
		}
	}
	if (count != 0)
		std::cout << "   " << count << " graphs updated.\n";
	sqlite3_exec(database, "COMMIT;", 0, 0, 0);

	sqlite3_finalize(qry);
	sqlite3_finalize(stmt);

	return true;
}


/**
 * checks the status table to determine if name graphs are already checked and labeled
**/
bool DatabaseInterface::checked(const char * specification, unsigned order) {
	if (order < 1)
	{
		std::cout << "Checking status failed. " << order << " is not a valid graph order." << std::endl;
		return true;
	}

	std::string query = "SELECT checked FROM Status WHERE specification == '" + std::string(specification) + "';";

	sqlite3_stmt * stmt;

	if (sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0) != SQLITE_OK)
	{
		std::cout << "Checking status failed. SQL error: invalid query." << std::endl;
		sqlite3_finalize(stmt);
		return false;
	}

	if (sqlite3_step(stmt) == SQLITE_ROW)
	{
		unsigned status = sqlite3_column_int(stmt, 0);
		unsigned power_of_2 = nth_power(2, order - 1);
		status %= 2 * power_of_2;
		status /= power_of_2;

		if (status == 1)
		{
			sqlite3_finalize(stmt);
			return true;
		}
	}
	else
		std::cout << "Checking status failed. Database error: " << specification << " does not occur in status table." << std::endl;

	sqlite3_finalize(stmt);
	return false;
}


/**
 * updates the status to indicate graphs of given order have been tested with regards to given specification
**/
void DatabaseInterface::update_status(const char * specification, unsigned order) {
	if (order < 1)
	{
		std::cout << "Updating status failed. " << order << " is not a valid graph order." << std::endl;
		return;
	}

	std::string query = "SELECT checked FROM Status WHERE specification == '" + std::string(specification) + "';";

	sqlite3_stmt * qry;

	if (sqlite3_prepare_v2(database, query.c_str(), -1, &qry, 0) != SQLITE_OK)
	{
		std::cout << "Updating status failed. SQL error: invalid query." << std::endl;
		sqlite3_finalize(qry);
		return;
	}

	if (sqlite3_step(qry) == SQLITE_ROW)
	{
		std::string statement = "UPDATE Status SET checked = " + std::to_string(sqlite3_column_int(qry, 0) + nth_power(2, order - 1)) + " WHERE specification == '" + specification + "';";
		execute_SQL_statement(&statement);
	}
	else
		std::cout << "Updating status failed. Database error: " << specification << " does not occur in status table." << std::endl;

	sqlite3_finalize(qry);
}