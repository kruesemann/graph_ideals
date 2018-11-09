#pragma once

#include "stdafx.h"


class BettiTable
{
public:
	unsigned columns;
	std::vector<unsigned> table;

	BettiTable() : columns(0), table{} {}


	bool read_next_table(std::ifstream* file);
	void read_from_line(std::string * line);

	std::string convert_to_line();

	unsigned get_projective_dimension();
	unsigned get_regularity();
	std::vector<unsigned> get_extremal_betti_numbers();
	std::string get_extremal_betti_numbers_as_string();

	std::vector<unsigned> get_column_widths();

	static std::string convert_tables_to_string(std::vector<BettiTable> * tables, std::vector<std::string> * table_names);
};
