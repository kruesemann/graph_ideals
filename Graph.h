#pragma once

#include "stdafx.h"


class Graph
{
private:
	unsigned order;
	unsigned * adjacencies;
	unsigned size;

	unsigned bron_kerbosch_pivot(std::vector<std::vector<unsigned>> * max_cliques, unsigned clique_number, std::vector<unsigned> * include_all, std::vector<unsigned> * include_some, std::vector<unsigned> * include_none);

	bool is_induced_connected(unsigned * vertices, unsigned subset_order);
	bool is_induced_path(int subset, unsigned subset_order);
	bool is_induced_claw(int subset);

	bool is_universal(unsigned vertex);
	bool is_simplicial(unsigned vertex, bool * visited);

	std::pair<unsigned, unsigned> get_simplicial_pair(bool * visited);

	std::pair<unsigned *, unsigned *> gen_lexicographic_ordering();

	bool peo_swappable(unsigned * peo, unsigned * h, int t);
	bool peo_move(unsigned * peo, unsigned * peo_indices, unsigned * h, int t);
	bool peo_switch(unsigned * peo, unsigned * peo_indices, unsigned * h, unsigned * a, unsigned * b, int t);
	bool test_pe_orderings(unsigned * peo, unsigned * peo_indices, unsigned * h, unsigned * a, unsigned * b, unsigned i);
	void gen_initial_peo(unsigned * peo, unsigned * peo_indices, unsigned * h, unsigned * a, unsigned * b);

public:
	Graph() : order(0), size(0) {
		adjacencies = 0;
	}

	Graph(unsigned order) : order(order), size(0) {
		if (order > 62)
			throw "tooManyVertices";

		adjacencies = new unsigned[order * order];

		for (unsigned i = 0; i < order * order; i++)
			adjacencies[i] = 0;
	}

	Graph(unsigned order, unsigned * adj);

	void read_graph_from_line(unsigned order, std::string * edges);
	void read_graph_from_vector(unsigned order, std::vector<std::pair<unsigned, unsigned>> * edges);

	Graph(unsigned order, std::string * edges) {
		adjacencies = 0;
		read_graph_from_line(order, edges);
	}

	Graph(unsigned order, std::vector<std::pair<unsigned, unsigned>> * edges) {
		adjacencies = 0;
		read_graph_from_vector(order, edges);
	}

	Graph& operator=(Graph && graph) {
		order = graph.order;
		graph.order = 0;
		adjacencies = graph.adjacencies;
		graph.adjacencies = 0;
		size = graph.size;
		graph.size = 0;
		return *this;
	}

	~Graph() {
		delete[] adjacencies;
		adjacencies = 0;
	}


	unsigned get_order();
	unsigned get_size();

	std::string convert_to_string();
	std::string convert_to_string_wrt_labeling(unsigned * labeling);
	std::string convert_to_g6_format();

	bool adjacent(unsigned v, unsigned w);

	bool read_next_g6_format(std::ifstream * file);
	bool read_next_list_format(std::ifstream * file);

	Graph get_complement();

	std::vector<unsigned> get_clique_numbers();
	std::vector<unsigned> get_detour_number();
	std::vector<unsigned> get_extreme_degrees();
	std::vector<unsigned> get_independence_numbers();
	std::vector<unsigned> get_girth();

	bool is_closed_wrt_labeling(unsigned * peo, unsigned * peo_indices);

	bool is_connected();
	bool is_cograph();
	bool is_euler();
	bool is_chordal();
	bool is_clawfree();
	bool is_closed();
	bool is_cone();

	unsigned * gen_closed_labeling();
};

