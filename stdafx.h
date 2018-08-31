#pragma once

#include <sstream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iostream>

#define PARSE_ERROR(X)	((std::cout << "\nParse error: " << X << std::endl),									(void)0)
#define INVALID_ARG()	((std::cout << "\nParse error: '" << arg << "' is not a valid argument." << std::endl),	(void)0)
#define WARNING(X)		((std::cout << "\nWarning: " << X << "\n"),												(void)0)
#define INPUT(X)		((std::cout << X << "\n>> "),															(void)0)
#define FAIL(X,Y)		((std::cout << X << " failed. " << Y << std::endl),										(void)0)
#define PROGRESS(X,Y)	((std::cout << std::string(X * 3, ' ') << Y << "...\n"),								(void)0)
#define RESULT(X)		((std::cout << X << std::endl),															(void)0)
#define SQL_ERROR(X)	((std::cout << "SQL error: '" << X << "' is not a valid statement." << std::endl),		(void)0)
#define SEPARATE()		((std::cout << "\n\n" << std::endl),													(void)0)