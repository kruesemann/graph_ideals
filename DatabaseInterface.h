#pragma once

#include <iostream>

#include "stdafx.h"
#include "sqlite3.h"

#include "Graph.h"
#include "BettiTable.h"


class DatabaseInterface
{
public:
	sqlite3 * database;

	unsigned number_columns;
	unsigned number_rows;

	std::vector<std::string> view_columns;
	std::vector<std::vector<std::string>> view_contents;
	
	std::vector<unsigned> column_widths;

	DatabaseInterface() {}

	DatabaseInterface(const char * database_file_name) : number_columns(0), number_rows(0), view_columns{}, view_contents{}, column_widths{} {
		if (sqlite3_open(database_file_name, &database))
		{
			std::cout << "Could not open database: " << sqlite3_errmsg(database) << std::endl;
			database = 0;
		}
	}

	DatabaseInterface& operator=(DatabaseInterface && dbi) {
		sqlite3_close_v2(database);
		database = dbi.database;
		dbi.database = 0;
		reset_view();
		return *this;
	}

	~DatabaseInterface() {
		sqlite3_close_v2(database);
	}

	void reset_view();
	void show_view(int limit = 100);
	void save_view(std::ofstream * file);
	void show_view_rich(int limit = 25);
	void save_view_rich(std::ofstream * file);
	void save_view_visualisation(std::ofstream * file);
	void save_view_g6(std::ofstream * file);
	void generate_m2_scripts(std::string * ideal_type, unsigned batch_size, std::vector<std::string> * required_packages);
	void generate_closed_labeling_m2_scripts(std::string * ideal_type, unsigned batch_size, std::vector<std::string> * required_packages);
	void generate_cone_lists();
	void compare_cone_regularities(std::string * filename);
	void show_status();

	bool execute_SQL_query(std::string * query);
	bool execute_SQL_statement(std::string * statement);
	bool create_status_table();
	bool create_graphs_table();
	void insert_graphs(std::ifstream * file);
	bool update_type(unsigned order, bool (Graph::*graph_test)(), const char * type, const char * query_condition);
	bool add_betti_data(unsigned order, std::string * name, std::string * ideal_type);
	bool add_closed_labeling_betti_data(unsigned order, std::string * name, std::string * ideal_type);
	bool checked(const char * name, unsigned order);
	void update_status(const char * specification, unsigned order);
};
