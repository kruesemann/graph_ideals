#include "test.h"

#include <chrono>

#define NUMBER_TESTS 23
#define PASSES 10
#define TESTFILE "graphs9.g6"

const char * TESTS[NUMBER_TESTS] = {
	"individual insert 1000",
	"single-transaction insert 1000",
	"single-transaction insert 1000000",
	"single-transaction batch insert 1000000",
	"individual update 1000",
	"single-transaction update 1000",
	"single-transaction update 1000000",
	"single-transaction prepared update 1000000",
	"connected BFS",
	"cograph naive",
	"euler naive",
	"chordal lexicographic BFS",
	"claw-free naive",
	"closed naive",
	"closed necessary condition",
	"closed iterating PEOs",
	"cone naive",
	"clique number naive",
	"clique numbers Bron-Kerbosch",
	"independence numbers complement Bron-Kerbosch",
	"detour number naive",
	"extremal numbers naive",
	"girth naive"
};


bool open_testfile(std::ifstream * file) {
	file->open(TESTFILE);

	if (!file->is_open())
	{
		FAIL("Testing", "Unable to open '" << TESTFILE << "'");
		return false;
	}
	return true;
}


void import_individual(DatabaseInterface * dbi, unsigned samplesize) {
	for (unsigned i = 0; i < samplesize; i++)
		dbi->execute_SQL_statement(&std::string("INSERT INTO Graphs (graphOrder, graphSize, edges) VALUES (1, 0, '{}');"));
}


void import_single_transaction(DatabaseInterface * dbi, unsigned samplesize) {
	sqlite3_exec(dbi->database, "BEGIN TRANSACTION;", 0, 0, 0);

	for (unsigned i = 0; i < samplesize; i++)
		dbi->execute_SQL_statement(&std::string("INSERT INTO Graphs (graphOrder, graphSize, edges) VALUES (1, 0, '{}');"));

	sqlite3_exec(dbi->database, "COMMIT;", 0, 0, 0);
}


void import_single_transaction_batch(DatabaseInterface * dbi, unsigned samplesize) {
	sqlite3_exec(dbi->database, "BEGIN TRANSACTION;", 0, 0, 0);

	for (unsigned i = 0; i < samplesize / 10; i++)
	{
		std::string statement = "INSERT INTO Graphs (graphOrder, graphSize, edges) VALUES ";

		for (unsigned j = 0; j < 10; j++)
			statement += "(1,0,'{}'),";

		statement.pop_back();
		dbi->execute_SQL_statement(&statement);
	}

	sqlite3_exec(dbi->database, "COMMIT;", 0, 0, 0);
}


void update_individual(DatabaseInterface * dbi, unsigned samplesize) {
	std::string query = "SELECT graphID FROM Graphs;";

	sqlite3_stmt * qry;

	if (sqlite3_prepare_v2(dbi->database, query.c_str(), -1, &qry, 0) != SQLITE_OK)
	{
		SQL_ERROR(query);
		FAIL("Labeling type", "");
		sqlite3_finalize(qry);
		return;
	}

	for (unsigned i = 0; i < samplesize && sqlite3_step(qry) == SQLITE_ROW; i++)
		dbi->execute_SQL_statement(&(std::string("UPDATE Graphs SET type = 'type' WHERE graphID == ") + std::to_string(sqlite3_column_int(qry, 0))));

	sqlite3_finalize(qry);
}


void update_single_transaction(DatabaseInterface * dbi, unsigned samplesize) {
	std::string query = "SELECT graphID FROM Graphs;";

	sqlite3_stmt * qry;

	if (sqlite3_prepare_v2(dbi->database, query.c_str(), -1, &qry, 0) != SQLITE_OK)
	{
		SQL_ERROR(query);
		FAIL("Labeling type", "");
		sqlite3_finalize(qry);
		return;
	}

	sqlite3_exec(dbi->database, "BEGIN TRANSACTION;", 0, 0, 0);
	for (unsigned i = 0; i < samplesize && sqlite3_step(qry) == SQLITE_ROW; i++)
		dbi->execute_SQL_statement(&(std::string("UPDATE Graphs SET type = 'type' WHERE graphID == ") + std::to_string(sqlite3_column_int(qry, 0))));

	sqlite3_exec(dbi->database, "COMMIT;", 0, 0, 0);

	sqlite3_finalize(qry);
}


void update_single_transaction_prepared(DatabaseInterface * dbi, unsigned samplesize) {
	std::string query = "SELECT graphID FROM Graphs;";

	sqlite3_stmt * qry;

	if (sqlite3_prepare_v2(dbi->database, query.c_str(), -1, &qry, 0) != SQLITE_OK)
	{
		SQL_ERROR(query);
		FAIL("Labeling type", "");
		sqlite3_finalize(qry);
		return;
	}

	sqlite3_stmt * stmt;

	if (sqlite3_prepare_v2(dbi->database, "UPDATE Graphs SET type = ? WHERE graphID == ?", -1, &stmt, 0) != SQLITE_OK)
	{
		SQL_ERROR("UPDATE Graphs SET type = ? WHERE graphID == ?");
		FAIL("Labeling type", "");
		sqlite3_finalize(qry);
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_exec(dbi->database, "BEGIN TRANSACTION;", 0, 0, 0);
	for (unsigned i = 0; i < samplesize && sqlite3_step(qry) == SQLITE_ROW; i++)
	{
		sqlite3_bind_text(stmt, 1, "type", -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 2, sqlite3_column_int(qry, 0));

		sqlite3_step(stmt);

		sqlite3_clear_bindings(stmt);
		sqlite3_reset(stmt);
	}
	sqlite3_exec(dbi->database, "COMMIT;", 0, 0, 0);

	sqlite3_finalize(qry);
	sqlite3_finalize(stmt);
}


void complete_test(DatabaseInterface * dbi) {
	std::ifstream file;
	std::chrono::duration<double, std::milli> t_total[NUMBER_TESTS];
	std::chrono::system_clock::time_point t;

	for (unsigned i = 0; i < NUMBER_TESTS; i++)
		t_total[i] = std::chrono::duration<double, std::milli>(0);

	std::cout << "Starting complete test.\n"
		<< "Inserting and updating will be tested in " << PASSES << " passes.\n"
		<< "Type labeling and value computation will be tested on all graphs in file '" << TESTFILE << "'.\n\n"
		<< "INSERT/UPDATE";

	unsigned nr = 0;

	for (unsigned i = 0; i < PASSES; i++)
	{
		nr = 0;
		std::cout << "\n\n\nPASS " << i + 1 << "\n\n";

		// IMPORT INDIVIDUAL 1000
		std::cout << TESTS[nr] << "...\n";
		dbi->execute_SQL_statement(&std::string("DROP TABLE Graphs;"));
		dbi->create_graphs_table();
		t = std::chrono::high_resolution_clock::now();
		import_individual(dbi, 1000);
		t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

		// IMPORT SINGLE-TRANSACTION 1000
		std::cout << TESTS[nr] << "...\n";
		dbi->execute_SQL_statement(&std::string("DROP TABLE Graphs;"));
		dbi->create_graphs_table();
		t = std::chrono::high_resolution_clock::now();
		import_single_transaction(dbi, 1000);
		t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

		// IMPORT SINGLE-TRANSACTION 1000000
		std::cout << TESTS[nr] << "...\n";
		dbi->execute_SQL_statement(&std::string("DROP TABLE Graphs;"));
		dbi->create_graphs_table();
		t = std::chrono::high_resolution_clock::now();
		import_single_transaction(dbi, 1000000);
		t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

		// IMPORT SINGLE-TRANSACTION BATCH 1000000
		std::cout << TESTS[nr] << "...\n";
		dbi->execute_SQL_statement(&std::string("DROP TABLE Graphs;"));
		dbi->create_graphs_table();
		t = std::chrono::high_resolution_clock::now();
		import_single_transaction_batch(dbi, 1000000);
		t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

		// UPDATE INDIVIDUAL 1000
		std::cout << TESTS[nr] << "...\n";
		t = std::chrono::high_resolution_clock::now();
		update_individual(dbi, 1000);
		t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

		// UPDATE SINGLE-TRANSACTION 1000
		std::cout << TESTS[nr] << "...\n";
		t = std::chrono::high_resolution_clock::now();
		update_single_transaction(dbi, 1000);
		t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

		// UPDATE SINGLE-TRANSACTION 1000000
		std::cout << TESTS[nr] << "...\n";
		t = std::chrono::high_resolution_clock::now();
		update_single_transaction(dbi, 1000000);
		t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

		// UPDATE SINGLE-TRANSACTION PREPARED 1000000
		std::cout << TESTS[nr] << "...\n";
		t = std::chrono::high_resolution_clock::now();
		update_single_transaction_prepared(dbi, 1000000);
		t_total[nr++] += std::chrono::high_resolution_clock::now() - t;
	}


	std::cout << "\n\n\nTYPE LABELING/VALUE COMPUTATION\n\n";

	// IMPORT TESTGRAPHS
	std::cout << "importing testgraphs...\n";
	dbi->execute_SQL_statement(&std::string("DROP TABLE Graphs;"));
	dbi->create_graphs_table();

	if (!open_testfile(&file))
		return;

	dbi->import_graphs(&file, &Graph::read_next_g6_format);
	file.close();

	// CONNECTED BFS
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_type(&Graph::is_connected, "connected", 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// COGRAPH NAIVE
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_type(&Graph::is_cograph, "cograph", 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// EULER NAIVE
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_type(&Graph::is_euler, "euler", 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// CHORDAL LEXICOGRAPHIC BFS
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_type(&Graph::is_chordal, "chordal", 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// CLAW-FREE NAIVE
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_type(&Graph::is_clawfree, "claw-free", 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// CLOSED NAIVE
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_type(&Graph::is_closed_naive, "clsdNaive", 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// CLOSED NECESSARY CONDITION
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_type(&Graph::is_closed_naive, "clsdNC", "TYPE LIKE '%chordal%' AND TYPE LIKE '%claw-free%'");
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// CLOSED ITERATING PEOS
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_type(&Graph::is_closed, "closed", "TYPE LIKE '%chordal%' AND TYPE LIKE '%claw-free%'");
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// CONE NAIVE
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_type(&Graph::is_cone, "cone", 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// CLIQUE NUMBER NAIVE
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_numbers(&Graph::get_clique_number_naive, &(std::vector<const char *>{ "clqNrNaive" }), 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// CLIQUE NUMBERS BRON-KERBOSCH
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_numbers(&Graph::get_clique_numbers, &(std::vector<const char *>{ "cliqueNumber", "maxCliqueNumber" }), 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// INDEPENDENCE NUMBERS
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_numbers(&Graph::get_independence_numbers, &(std::vector<const char *>{ "stableNumber", "maxStableNumber" }), 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// DETOUR NUMBER
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_numbers(&Graph::get_detour_number, &(std::vector<const char *>{ "detourNumber" }), 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// EXTREMAL DEGREES
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_numbers(&Graph::get_extreme_degrees, &(std::vector<const char *>{ "minDeg", "maxDeg" }), 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	// GIRTH
	std::cout << TESTS[nr] << "...\n";
	t = std::chrono::high_resolution_clock::now();
	dbi->update_numbers(&Graph::get_girth, &(std::vector<const char *>{ "girth" }), 0);
	t_total[nr++] += std::chrono::high_resolution_clock::now() - t;

	std::cout << "\n\n\nRESULTS\n\n";

	for (unsigned i = 0; i < NUMBER_TESTS; i++)
		std::cout << TESTS[i] << "\ntime: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_total[i]).count() << "\n\n";
}
