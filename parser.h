#pragma once

#include "stdafx.h"
#include "DatabaseInterface.h"


std::string cut_first_argument(std::string * input);
int parse_unsigned(std::string * arg);

void help_parse		(DatabaseInterface * dbi, std::string * input);
void import_parse	(DatabaseInterface * dbi, std::string * input);
void values_parse	(DatabaseInterface * dbi, std::string * input);
void type_parse		(DatabaseInterface * dbi, std::string * input);
void script_parse	(DatabaseInterface * dbi, std::string * input);
void results_parse	(DatabaseInterface * dbi, std::string * input);
void show_parse		(DatabaseInterface * dbi, std::string * input);
void save_parse		(DatabaseInterface * dbi, std::string * input);