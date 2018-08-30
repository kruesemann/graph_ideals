#include "Graph.h"

#include <list>


//########## helper functions ##########
/**
 * returns the appropriate index of the array in which the adjacency matrix is stored
**/
inline unsigned get_index(unsigned first_vertex, unsigned second_vertex, unsigned order) {
	return (first_vertex - 1) * order + second_vertex - 1;
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
 * refines the partitioning by splitting every partition depending on set,
 * used for generating a lexicographic labeling
**/
void refine(std::list<std::vector<unsigned>> * partitions, bool * set) {
	std::list<std::vector<unsigned>>::iterator i;

	for (i = partitions->begin(); i != partitions->end();)
	{
		std::vector<unsigned> intersection = {};
		std::vector<unsigned> difference = {};

		for (unsigned vertex : *i)
		{
			if (set[vertex - 1])
				intersection.push_back(vertex);
			else
				difference.push_back(vertex);
		}

		i = partitions->erase(i);
		if (!intersection.empty())
			partitions->insert(i, intersection);
		if (!difference.empty())
			partitions->insert(i, difference);
	}
}


//########## private member functions ##########
/**
* recursively adds all inclusion-maximal cliques to max_cliques and returns the current clique number via Bron-Kerbosch algorithm with pivots
**/
unsigned Graph::bron_kerbosch_pivot(std::vector<std::vector<unsigned>> * max_cliques, unsigned clique_number, std::vector<unsigned> * include_all, std::vector<unsigned> * include_some, std::vector<unsigned> * include_none) {
	if (include_some->empty()
		&& include_none->empty())
	{
		max_cliques->push_back(*include_all);
		unsigned size = include_all->size();
		return clique_number < size ? size : clique_number;
	}

	unsigned new_clique_number = clique_number;

	unsigned pivot = !include_some->empty() ? include_some->back() : include_all->back();
	for (unsigned i = 0; i < include_some->size();)
	{
		unsigned v = include_some->at(i);
		if (!adjacencies[get_index(pivot, v, order)])
		{
			std::vector<unsigned> new_all;
			std::vector<unsigned> new_some;
			std::vector<unsigned> new_none;
			unsigned all_size = include_all->size();
			unsigned some_size = include_some->size();
			unsigned none_size = include_none->size();

			for (unsigned j = 0; j < order; j++)
			{
				if (j < all_size)
					new_all.push_back(include_all->at(j));

				if (j < some_size)
				{
					unsigned w = include_some->at(j);
					if (adjacencies[get_index(v, w, order)])
						new_some.push_back(w);
				}

				if (j < none_size)
				{
					unsigned w = include_none->at(j);
					if (adjacencies[get_index(v, w, order)])
						new_none.push_back(w);
				}
			}
			new_all.push_back(v);

			unsigned recursive_clique_number = bron_kerbosch_pivot(max_cliques, clique_number, &new_all, &new_some, &new_none);
			if (new_clique_number < recursive_clique_number)
				new_clique_number = recursive_clique_number;

			include_some->erase(include_some->begin() + i);
			include_none->push_back(v);
		}
		else
			i++;
	}

	return new_clique_number;
}


/**
 * generates all order-4-subsets of vertices
**/
std::vector<unsigned> Graph::gen_order_4_subsets() {
	std::vector<unsigned> subs = {};

	for (unsigned i = 1; i <= order - 3; i++)
	{
		for (unsigned j = i + 1; j <= order - 2; j++)
		{
			for (unsigned k = j + 1; k <= order - 1; k++)
			{
				for (unsigned m = k + 1; m <= order; m++)
				{
					subs.push_back(i);
					subs.push_back(j);
					subs.push_back(k);
					subs.push_back(m);
				}
			}
		}
	}

	return subs;
}


/**
 * tests if a given order-4-subset of vertices is an induced path
**/
bool Graph::is_induced_path(std::vector<unsigned> * order_4_subsets, unsigned subset_nr) {
	unsigned degrees[] = { 0, 0, 0, 0 };
	unsigned subset[] = { order_4_subsets->at(subset_nr * 4), order_4_subsets->at(subset_nr * 4 + 1), order_4_subsets->at(subset_nr * 4 + 2), order_4_subsets->at(subset_nr * 4 + 3) };

	for (unsigned i = 0; i < 4; i++)
	{
		for (unsigned j = i + 1; j < 4; j++)
		{
			if (adjacencies[get_index(subset[i], subset[j], order)])
			{
				degrees[i]++;
				degrees[j]++;
			}
		}
	}

	unsigned check_sum_degrees = 0;

	for (unsigned i = 0; i < 4; i++)
	{
		if (degrees[i] != 1
			&& degrees[i] != 2)
			return false;
		check_sum_degrees += degrees[i];
	}

	return check_sum_degrees == 6;
}


/**
 * generates a lexicographic labeling on the vertices of the graph via lexicographic-breadth-first-search
**/
std::pair<unsigned *, unsigned *> Graph::gen_lexicographic_labeling() {
	if (order == 0)
		return {};

	std::list<std::vector<unsigned>> partitions;
	std::vector<unsigned> initial_set;

	bool * visited = new bool[order];

	for (unsigned vertex = 1; vertex <= order; vertex++)
	{
		if (vertex == 1)
			partitions.push_front({ 1 });
		else
			initial_set.push_back(vertex);

		visited[vertex - 1] = false;
	}
	partitions.push_back(initial_set);

	std::list<std::vector<unsigned>>::iterator i;
	unsigned * labeling = new unsigned[order];

	for (unsigned j = 0; j < order; j++)
	{
		i = partitions.begin();
		unsigned vertex = i->back();
		i->pop_back();
		if (i->empty())
			partitions.erase(i);

		visited[vertex - 1] = true;
		labeling[j] = vertex;

		bool * set = new bool[order];
		for (unsigned neighbour = 1; neighbour <= order; neighbour++)
		{
			if (!visited[neighbour - 1]
				&& adjacencies[get_index(vertex, neighbour, order)])
				set[neighbour - 1] = true;
			else
				set[neighbour - 1] = false;
		}

		refine(&partitions, set);
		delete[] set;
	}

	unsigned * labeling_indices = new unsigned[order];

	for (unsigned j = 0; j < order; j++)
		labeling_indices[labeling[j] - 1] = j;

	delete[] visited;
	return std::pair<unsigned *, unsigned *>(labeling, labeling_indices);
}


/**
* tests if a given order-4-subset of vertices is an induced claw
**/
bool Graph::is_induced_claw(std::vector<unsigned> * order_4_subsets, unsigned subset_nr) {
	unsigned degrees[] = { 0, 0, 0, 0 };
	unsigned subset[] = { order_4_subsets->at(subset_nr * 4), order_4_subsets->at(subset_nr * 4 + 1), order_4_subsets->at(subset_nr * 4 + 2), order_4_subsets->at(subset_nr * 4 + 3) };

	for (unsigned i = 0; i < 4; i++)
	{
		for (unsigned j = i + 1; j < 4; j++)
		{
			if (adjacencies[get_index(subset[i], subset[j], order)])
			{
				degrees[i]++;
				degrees[j]++;
			}
		}
	}

	unsigned check_sum_degrees = 0;

	for (unsigned i = 0; i < 4; i++)
	{
		if (degrees[i] != 1
			&& degrees[i] != 3)
			return false;
		check_sum_degrees += degrees[i];
	}

	return check_sum_degrees == 6;
}


/**
 * tests if the graph is closed with regards to given (perfect elimination) ordering, i.e.
 * {a,b},{i,j} in E(G) with a < b, i < j  =>  {a,i} in E(G) if b=j and {b,j} in E(G) if a=i
**/
bool Graph::is_closed_wrt_labeling(unsigned * peo, unsigned * peo_indices) {
	for (unsigned vertex = 1; vertex <= order; vertex++)
	{
		int nearest_prior_neighbour;
		for (nearest_prior_neighbour = (int)(peo_indices[vertex - 1]) - 1; nearest_prior_neighbour >= 0; nearest_prior_neighbour--)
		{
			if (adjacencies[get_index(vertex, peo[nearest_prior_neighbour], order)])
				break;
		}

		if (nearest_prior_neighbour >= 0)
		{
			for (int prior_neighbour = 0; prior_neighbour < nearest_prior_neighbour; prior_neighbour++)
			{
				if (adjacencies[get_index(vertex, peo[prior_neighbour], order)]
					&& !adjacencies[get_index(peo[nearest_prior_neighbour], peo[prior_neighbour], order)])
					return false;
			}
		}

		int nearest_later_neighbour;
		for (nearest_later_neighbour = (int)(peo_indices[vertex - 1]) + 1; nearest_later_neighbour < (int)order; nearest_later_neighbour++)
		{
			if (adjacencies[get_index(vertex, peo[nearest_later_neighbour], order)])
				break;
		}

		if (nearest_later_neighbour < (int)order)
		{
			for (int later_neighbour = order - 1; later_neighbour > nearest_later_neighbour; later_neighbour--)
			{
				if (adjacencies[get_index(vertex, peo[later_neighbour], order)]
					&& !adjacencies[get_index(peo[nearest_later_neighbour], peo[later_neighbour], order)])
					return false;
			}
		}
	}

	return true;
}


/**
* tests if the vertices in given perfect elimination ordering at index t and t+1 are swappable, i.e. the peo with these vertices swapped is a peo as well
**/
bool Graph::peo_swappable(unsigned * peo, unsigned * h, int t) {
	if (t == ((int)order) - 1)
		return false;

	unsigned x = peo[t];
	unsigned y = peo[t + 1];

	return !adjacencies[get_index(x, y, order)] || h[x - 1] == h[y - 1] + 1;
}


/**
* swaps the vertices in given given perfect elimination ordering at index t and t+1 and tests if the graph is closed with regards to this ordering
**/
bool Graph::peo_move(unsigned * peo, unsigned * peo_indices, unsigned * h, int t) {
	unsigned x = peo[t];
	unsigned y = peo[t + 1];

	peo[t] = y;
	peo[t + 1] = x;
	unsigned temp = peo_indices[x - 1];
	peo_indices[x - 1] = peo_indices[y - 1];
	peo_indices[y - 1] = temp;

	if (adjacencies[get_index(x, y, order)])
	{
		h[x - 1]--;
		h[y - 1]++;
	}

	return is_closed_wrt_labeling(peo, peo_indices);
}


/**
* a special swap where the order of the t-th pair of simplicial vertices is swapped
* also tests if the graph is closed with regards to the new labeling
**/
bool Graph::peo_switch(unsigned * peo, unsigned * peo_indices, unsigned * h, unsigned * a, unsigned * b, int t) {
	if (t != -1)
	{
		unsigned x = a[t];
		unsigned y = b[t];

		unsigned temp = peo[peo_indices[x - 1]];
		peo[peo_indices[x - 1]] = peo[peo_indices[y - 1]];
		peo[peo_indices[y - 1]] = temp;
		temp = peo_indices[x - 1];
		peo_indices[x - 1] = peo_indices[y - 1];
		peo_indices[y - 1] = temp;
		temp = a[t];
		a[t] = b[t];
		b[t] = temp;

		if (adjacencies[get_index(x, y, order)])
		{
			h[x - 1]--;
			h[y - 1]++;
		}
	}

	return is_closed_wrt_labeling(peo, peo_indices);
}


/**
* recursively iterates through all perfect elimination orderings of the graph (twice) by swapping consecutive elements
* returns true as soon as an ordering was found with respect to which the graph is closed
**/
bool Graph::test_pe_orderings(unsigned * peo, unsigned * peo_indices, unsigned * h, unsigned * a, unsigned * b, unsigned i) {
	if (i == 0)
		return false;

	if (test_pe_orderings(peo, peo_indices, h, a, b, i - 1))
		return true;

	unsigned mrb = 0;
	bool typical = false;

	while (peo_swappable(peo, h, peo_indices[b[i - 1] - 1]))
	{
		mrb++;
		if (peo_move(peo, peo_indices, h, peo_indices[b[i - 1] - 1])
			|| test_pe_orderings(peo, peo_indices, h, a, b, i - 1))
			return true;

		unsigned mra = 0;
		if (peo[peo_indices[a[i - 1] - 1] + 1] != b[i - 1]
			&& peo_swappable(peo, h, peo_indices[a[i - 1] - 1]))
		{
			typical = true;
			do
			{
				mra++;
				if (peo_move(peo, peo_indices, h, peo_indices[a[i - 1] - 1])
					|| test_pe_orderings(peo, peo_indices, h, a, b, i - 1))
					return true;

			} while (peo[peo_indices[a[i - 1] - 1] + 1] != b[i - 1]
				&& peo_swappable(peo, h, peo_indices[a[i - 1] - 1]));
		}

		if (typical)
		{
			if (peo_switch(peo, peo_indices, h, a, b, i - 2)
				|| test_pe_orderings(peo, peo_indices, h, a, b, i - 1))
				return true;

			unsigned mla;
			if (mrb % 2 == 1)
				mla = mra - 1;
			else
				mla = mra + 1;

			for (unsigned j = 1; j <= mla; j++)
			{
				if (peo_move(peo, peo_indices, h, peo_indices[a[i - 1] - 1] - 1)
					|| test_pe_orderings(peo, peo_indices, h, a, b, i - 1))
					return true;
			}
		}
	}

	if (typical
		&& mrb % 2 == 1)
	{
		if (peo_move(peo, peo_indices, h, peo_indices[a[i - 1] - 1] - 1))
			return true;
	}
	else if (peo_switch(peo, peo_indices, h, a, b, i - 2))
		return true;

	if (test_pe_orderings(peo, peo_indices, h, a, b, i - 1))
		return true;

	for (unsigned j = 1; j <= mrb; j++)
	{
		if (peo_move(peo, peo_indices, h, peo_indices[b[i - 1] - 1] - 1)
			|| test_pe_orderings(peo, peo_indices, h, a, b, i - 1))
			return true;
	}

	return false;
}


/**
* tests if the given vertex is simplicial with respect to the subgraph induced by the vertices which have not been visited
**/
bool Graph::is_simplicial(unsigned vertex, bool * visited) {
	for (unsigned v = 1; v <= order; v++)
	{
		if (!visited[v - 1]
			&& adjacencies[get_index(vertex, v, order)])
		{
			for (unsigned w = v + 1; w <= order; w++)
			{
				if (!visited[w - 1]
					&& adjacencies[get_index(vertex, w, order)]
					&& !adjacencies[get_index(v, w, order)])
					return false;
			}
		}
	}

	return true;
}


/**
* returns a pair of simplicial vertices of the subgraph induced by the vertices which have not been visited
* expects the subgraph to have at least two simplicial vertices (this is the case if the graph is chordal for example)
**/
std::pair<unsigned, unsigned> Graph::get_simplicial_pair(bool * visited) {
	unsigned first = 0;
	unsigned second = 0;

	for (unsigned v = 1; v <= order; v++)
	{
		if (!visited[v - 1]
			&& is_simplicial(v, visited))
		{
			if (first == 0)
				first = v;
			else
			{
				second = v;
				break;
			}
		}
	}

	return std::pair<unsigned, unsigned>(first, second);
}


/**
* generates an initial perfect elimination ordering via successive elimination of pairs of simplicial vertices
**/
void Graph::gen_initial_peo(unsigned * peo, unsigned * peo_indices, unsigned * h, unsigned * a, unsigned * b) {
	bool * visited = new bool[order];
	for (unsigned i = 0; i < order; i++)
		visited[i] = false;

	for (unsigned i = 1; i <= order / 2; i++)
	{
		std::pair<unsigned, unsigned> simplicial_pair = get_simplicial_pair(visited);
		visited[simplicial_pair.first - 1] = true;
		visited[simplicial_pair.second - 1] = true;

		a[i - 1] = simplicial_pair.first;
		b[i - 1] = simplicial_pair.second;
		peo[2 * i - 2] = simplicial_pair.first;
		peo[2 * i - 1] = simplicial_pair.second;
		peo_indices[simplicial_pair.first - 1] = 2 * i - 2;
		peo_indices[simplicial_pair.second - 1] = 2 * i - 1;
	}

	if (order % 2 == 1)
	{
		unsigned x = 0;
		for (unsigned i = 0; i < order; i++)
		{
			if (!visited[i])
			{
				x = i + 1;
				break;
			}
		}

		peo[order - 1] = x;
		peo_indices[x - 1] = order - 1;
	}

	for (unsigned v = 1; v <= order; v++)
	{
		unsigned neighbour_count = 0;
		for (unsigned i = peo_indices[v - 1] + 1; i < order; i++)
		{
			if (adjacencies[get_index(v, peo[i], order)])
				neighbour_count++;
		}
		h[v - 1] = neighbour_count;
	}

	delete[] visited;
}


/**
 * tests if given vertex is universal, i.e. it is adjacent to all other vertices of the graph
**/
bool Graph::is_universal(unsigned vertex) {
	for (unsigned w = 1; w <= order; w++)
		if (w != vertex
			&& !adjacencies[get_index(vertex, w, order)])
			return false;

	return true;
}


//########## public member functions ##########
/**
 * constructor
**/
Graph::Graph(unsigned order, unsigned * adj) {
	if (order > 62)
	{
		RESULT("Graph has too many vertices.");
		throw "tooManyVertices";
	}

	this->order = order;
	adjacencies = adj;
	adj = 0;
	size = 0;

	for (unsigned v = 1; v <= order; v++)
		for (unsigned w = v + 1; w <= order; w++)
			if (adjacencies[get_index(v, w, order)] != 0)
				size++;
}


/**
 * expects a list of edges of the following form "{{1,2},{1,4},{2,3},{3,5}}"
**/
void Graph::read_graph_from_line(unsigned order, std::string * edges) {
	if (order > 62)
	{
		FAIL("Reading graph from line", "Graph has too many vertices.");
		return;
	}

	delete[] adjacencies;

	unsigned length = edges->length();
	if (length < 2)
	{
		PARSE_ERROR("List of edges incomplete.");
		FAIL("Reading graph from line", "");
		return;
	}

	this->order = order;
	adjacencies = new unsigned[order * order];
	size = 0;

	for (unsigned i = 0; i < order * order; i++)
		adjacencies[i] = 0;

	for (unsigned i = 1; i < length - 1;)
	{
		if (edges->at(i) != '{')
		{
			PARSE_ERROR("Expected '{', got '" << edges->at(i) << "' instead.");
			FAIL("Reading graph from line", "");
			return;
		}
		i++;
		
		unsigned first_vertex = 0;
		while (i < length && edges->at(i) != ',')
		{
			first_vertex *= 10;
			first_vertex += edges->at(i) - '0';
			i++;
		}
		i++;

		unsigned second_vertex = 0;
		while (i < length && edges->at(i) != '}')
		{
			second_vertex *= 10;
			second_vertex += edges->at(i) - '0';
			i++;
		}
		i++;

		if (first_vertex < 1 || first_vertex > order
			|| second_vertex < 1 || second_vertex > order)
		{
			FAIL("Reading graph from line", "Illegal edge.");
			return;
		}

		adjacencies[get_index(first_vertex, second_vertex, order)] = 1;
		adjacencies[get_index(second_vertex, first_vertex, order)] = 1;
		size++;

		if (i >= length
			|| (edges->at(i) != ',' && edges->at(i) != '}')
			|| (i == length - 2 && edges->at(i) == ','))
		{
			PARSE_ERROR("Line incomplete.");
			FAIL("Reading graph from line", "");
			return;
		}
		i++;
	}
}


/**
 * expects a vector of edges
**/
void Graph::read_graph_from_vector(unsigned order, std::vector<std::pair<unsigned, unsigned>> * edges) {
	if (order > 62)
	{
		FAIL("Reading graph from vector", "Graph has too many vertices.");
		return;
	}

	delete[] adjacencies;

	this->order = order;
	adjacencies = new unsigned[order * order];
	size = 0;

	for (unsigned i = 0; i < order * order; i++)
		adjacencies[i] = 0;

	for (unsigned i = 0; i < edges->size(); i++)
	{
		std::pair<unsigned, unsigned> edge = edges->at(i);

		if (edge.first < 1 || edge.first > order
			|| edge.second < 1 || edge.second > order)
		{
			FAIL("Reading graph from vector", "Illegal edge.");
			return;
		}

		adjacencies[get_index(edge.first, edge.second, order)] = 1;
		adjacencies[get_index(edge.second, edge.first, order)] = 1;
		size++;
	}
}


/**
 * returns number of vertices
**/
unsigned Graph::get_order() {
	return order;
}


/**
 * converts the adjacency matrix into a string containing all edges in the form of "{{1,2},{1,4},{2,3},{3,5}}"
**/
std::string Graph::convert_to_string() {
	std::string edges = "{";

	for (unsigned first_vertex = 1; first_vertex <= order; first_vertex++)
	{
		for (unsigned second_vertex = first_vertex + 1; second_vertex <= order; second_vertex++)
		{
			if (adjacencies[get_index(first_vertex, second_vertex, order)])
			{
				edges.push_back('{');
				edges += std::to_string(first_vertex);
				edges.push_back(',');
				edges += std::to_string(second_vertex);
				edges.push_back('}');
				edges.push_back(',');
			}
		}
	}

	if (edges.length() > 1)
		edges.pop_back();

	edges.push_back('}');

	return edges;
}



std::string Graph::convert_to_string_wrt_labeling(unsigned * labeling_indices) {
	std::string edges = "{";

	for (unsigned first_vertex = 1; first_vertex <= order; first_vertex++)
	{
		for (unsigned second_vertex = first_vertex + 1; second_vertex <= order; second_vertex++)
		{
			if (adjacencies[get_index(first_vertex, second_vertex, order)])
			{
				edges.push_back('{');
				edges += std::to_string(labeling_indices[first_vertex - 1] + 1);
				edges.push_back(',');
				edges += std::to_string(labeling_indices[second_vertex - 1] + 1);
				edges.push_back('}');
				edges.push_back(',');
			}
		}
	}

	if (edges.length() > 1)
		edges.pop_back();

	edges.push_back('}');

	return edges;
}


/**
 * converts the graph to a string using the g6 format
**/
std::string Graph::convert_to_g6_format() {
	std::string g6_string = "";
	if (order < 63)
		g6_string.push_back((char)order + 63);

	std::vector<unsigned> bit_adjacencies = {};

	for (unsigned w = order; w > 1; w--)
	{
		for (unsigned v = w - 1; v > 0; v--)
			bit_adjacencies.push_back(adjacencies[get_index(v, w, order)]);
	}

	while (!bit_adjacencies.empty())
	{
		unsigned byte_value = 63;
		unsigned i;

		for (i = 6; i > 0 && !bit_adjacencies.empty(); i--)
		{
			byte_value += nth_power(2 * bit_adjacencies.back(), i - 1);
			bit_adjacencies.pop_back();
		}

		g6_string.push_back((char)byte_value);

		if (i > 0)
			break;
	}

	return g6_string;
}


/**
* returns true if v and w are adjacent vertices
**/
bool Graph::adjacent(unsigned v, unsigned w) {
	return adjacencies[get_index(v, w, order)] == 1;
}


/**
 * returns number of edges
**/
unsigned Graph::get_size() {
	return size;
}


/**
 * expects a file where graphs are parsed as adjacency lists as generated by nauty's showg. The following is an example of such a file:
 *						//empty line is important
 * Graph 1, order 3.	//The content of this line is ignored
 *   1 : 3;
 *   2 : 3;
 *   3 : 1 2;
 *
 * Graph 2, order 3.
 *   1 : 2 3;
 *   2 : 1 3;
 *   3 : 1 2;
 *
**/
bool Graph::read_next_adjacency_format(std::ifstream * file) {
	std::string line = "";
	while (getline(*file, line) && line.empty());

	if (line.empty())
		return false;

	unsigned first_vertex;
	std::vector<std::pair<unsigned, unsigned>> edges;

	for (first_vertex = 1; getline(*file, line) && !line.empty(); first_vertex++)
	{
		line.pop_back();
		line = line.substr(line.find_first_of(':') + 2, std::string::npos);

		for (unsigned i = 0; i < line.length(); i++)
		{
			unsigned second_vertex = 0;

			while (i < line.length() && line.at(i) != ' ')
			{
				second_vertex *= 10;
				second_vertex += line.at(i) - '0';
				i++;
			}

			if (first_vertex < second_vertex)
			{
				edges.push_back(std::pair<unsigned, unsigned>(first_vertex, second_vertex));
				size++;
			}
		}
	}

	read_graph_from_vector(first_vertex - 1, &edges);

	return true;
}


/**
 * expects a file where graphs are parsed in g6-format as generated by nauty
**/
bool Graph::read_next_g6_format(std::ifstream * file) {
	std::string g6_string;
	if (!getline(*file, g6_string) || g6_string.empty())
		return false;

	unsigned n = (unsigned)g6_string.front();
	if (n == 126)
	{
		FAIL("Reading g6 graph", "Graph has too many vertices.");
		return false;
	}

	std::vector<unsigned> bit_adjacencies;
	for (unsigned i = 1; i < g6_string.length(); i++)
	{
		unsigned byte_value = ((unsigned)g6_string.at(i)) - 63;

		for (unsigned j = 6; j > 0; j--)
		{
			unsigned power = nth_power(2, j - 1);

			if (power <= byte_value)
			{
				bit_adjacencies.push_back(1);
				byte_value -= power;
			}
			else
				bit_adjacencies.push_back(0);
		}
	}

	std::vector<std::pair<unsigned, unsigned>> edges;
	unsigned v = 1;
	unsigned w = 2;
	for (unsigned i = 0; i < bit_adjacencies.size(); i++)
	{
		if (bit_adjacencies.at(i) == 1)
			edges.push_back(std::pair<unsigned, unsigned>(v, w));

		v++;
		if (v == w)
		{
			v = 1;
			w++;
		}
	}

	read_graph_from_vector(n - 63, &edges);

	return true;
}


/**
* expects a file where graphs are parsed in a line with vertex number and edge-list, e.g., 3 {1,2},{2,3}
**/
bool Graph::read_next_list_format(std::ifstream * file) {
	std::string line = "";
	while (getline(*file, line) && line.empty());

	if (line.empty())
		return false;

	std::vector<std::pair<unsigned, unsigned>> edges;
	unsigned ord = 0;
	size = 0;

	unsigned i;
	for (i = 0; i < line.length() && line.at(i) != ' '; i++)
	{
		ord *= 10;
		ord += line.at(i) - '0';
	}

	if (i < line.length() && line.at(i) != ' ')
	{
		PARSE_ERROR("Expected ' ', got '" << line.at(i) << "' instead.");
		FAIL("Reading graph from list", "");
		return false;
	}
	i++;

	while (i < line.length())
	{
		unsigned first_vertex = 0;
		unsigned second_vertex = 0;

		if (line.at(i) != '{')
		{
			PARSE_ERROR("Expected '{', got '" << line.at(i) << "' instead.");
			FAIL("Reading graph from list", "");
			return false;
		}
		i++;

		while (i < line.length() && line.at(i) != ',')
		{
			first_vertex *= 10;
			first_vertex += line.at(i) - '0';
			i++;
		}

		if (i >= line.length() || line.at(i) != ',')
		{
			PARSE_ERROR("Expected ','.");
			FAIL("Reading graph from list", "");
			return false;
		}
		i++;

		while (i < line.length() && line.at(i) != '}')
		{
			second_vertex *= 10;
			second_vertex += line.at(i) - '0';
			i++;
		}

		if (i >= line.length() || line.at(i) != '}')
		{
			PARSE_ERROR("Expected '}'.");
			FAIL("Reading graph from list", "");
			return false;
		}
		i++;
		if (i < line.length() && line.at(i) != ',')
		{
			PARSE_ERROR("Expected ',', got '" << line.at(i) << "' instead.");
			FAIL("Reading graph from list", "");
			return false;
		}
		i++;

		edges.push_back(std::pair<unsigned, unsigned>(first_vertex, second_vertex));
		size++;
	}

	read_graph_from_vector(ord, &edges);

	return true;
}


/**
* returns the clique number and the number of inclusion-maximal cliques
**/
std::vector<unsigned> Graph::get_clique_numbers() {
	std::vector<std::vector<unsigned>> max_cliques;
	std::vector<unsigned> include_all;
	std::vector<unsigned> include_some;
	std::vector<unsigned> include_none;
	for (unsigned v = 1; v <= order; v++)
		include_some.push_back(v);

	unsigned clique_number = bron_kerbosch_pivot(&max_cliques, 0, &include_all, &include_some, &include_none);

	return { clique_number, max_cliques.size() };
}


/**
 * tests if the graph is connected via depth-first-search
**/
bool Graph::is_connected() {
	if (order < 2)
		return true;

	bool * visited = new bool[order];
	std::vector<unsigned> stack = { 1 };

	visited[0] = true;
	for (unsigned i = 1; i < order; i++)
		visited[i] = false;

	while (!stack.empty())
	{
		unsigned current = stack.back();
		stack.pop_back();

		for (unsigned neighbour = 1; neighbour <= order; neighbour++)
		{
			if (!visited[neighbour - 1]
				&& adjacencies[get_index(current, neighbour, order)])
			{
				stack.push_back(neighbour);
				visited[neighbour - 1] = true;
			}
		}
	}

	for (unsigned i = 1; i < order; i++)
	{
		if (!visited[i])
		{
			delete[] visited;
			return false;
		}
	}

	delete[] visited;
	return true;
}


/**
 * tests if the graph is a cograph, i.e. it has no induced path of length 3
**/
bool Graph::is_cograph() {
	if (order < 4)
		return true;

	std::vector<unsigned> order_4_subsets = gen_order_4_subsets();

	for (unsigned i = 0; i < order_4_subsets.size() / 4; i++)
	{
		if (is_induced_path(&order_4_subsets, i))
			return false;
	}

	return true;
}


/**
 * tests if the graph is euler, i.e. it has no vertices of odd degree
**/
bool Graph::is_euler() {
	if (order == 0)
		return true;

	unsigned * degrees = new unsigned[order];

	for (unsigned i = 0; i < order; i++)
		degrees[i] = 0;

	for (unsigned first_vertex = 1; first_vertex <= order; first_vertex++)
	{
		for (unsigned second_vertex = first_vertex + 1; second_vertex <= order; second_vertex++)
		{
			if (adjacencies[get_index(first_vertex, second_vertex, order)])
			{
				degrees[first_vertex - 1]++;
				degrees[second_vertex - 1]++;
			}
		}
	}

	for (unsigned i = 0; i < order; i++)
	{
		if (degrees[i] % 2 == 1)
		{
			delete[] degrees;
			return false;
		}
	}

	delete[] degrees;
	return true;
}


/**
 * tests if the graph is chordal, i.e. its vertices have a perfect elimination ordering
**/
bool Graph::is_chordal() {
	if (order < 4)
		return true;

	std::pair<unsigned *, unsigned *> labeling = gen_lexicographic_labeling();

	for (unsigned vertex = 1; vertex <= order; vertex++)
	{
		int nearest_prior_neighbour;
		for (nearest_prior_neighbour = (int)(labeling.second[vertex - 1]) - 1; nearest_prior_neighbour >= 0; nearest_prior_neighbour--)
		{
			if (adjacencies[get_index(vertex, labeling.first[nearest_prior_neighbour], order)])
				break;
		}

		if (nearest_prior_neighbour >= 0)
		{
			for (int prior_neighbour = 0; prior_neighbour < nearest_prior_neighbour; prior_neighbour++)
			{
				if (adjacencies[get_index(vertex, labeling.first[prior_neighbour], order)]
					&& !adjacencies[get_index(labeling.first[nearest_prior_neighbour], labeling.first[prior_neighbour], order)])
				{
					delete[] labeling.first;
					delete[] labeling.second;
					return false;
				}
			}
		}
	}

	delete[] labeling.first;
	delete[] labeling.second;
	return true;
}


/**
 * tests if the graph is claw-free, i.e. it has no complete 1-3-bipartite induced subgraph
**/
bool Graph::is_clawfree() {
	if (order < 4)
		return true;

	std::vector<unsigned> order_4_subsets = gen_order_4_subsets();

	for (unsigned i = 0; i < order_4_subsets.size() / 4; i++)
	{
		if (is_induced_claw(&order_4_subsets, i))
			return false;
	}

	return true;
}


/**
 * expects the graph to be chordal
 * tests if the graph is closed, i.e. there exists an labeling with respect to which the graph is closed
**/
bool Graph::is_closed() {
	if (order < 3)
		return true;

	bool closed = false;
	unsigned * peo = new unsigned[order];
	unsigned * peo_indices = new unsigned[order];
	unsigned * h = new unsigned[order];
	unsigned * a = new unsigned[order / 2];
	unsigned * b = new unsigned[order / 2];

	gen_initial_peo(peo, peo_indices, h, a, b);
	closed = is_closed_wrt_labeling(peo, peo_indices);

	if (!closed)
		closed = test_pe_orderings(peo, peo_indices, h, a, b, order / 2);

	if (!closed)
		closed = peo_switch(peo, peo_indices, h, a, b, order / 2 - 1);

	if (!closed)
		closed = test_pe_orderings(peo, peo_indices, h, a, b, order / 2);

	delete[] peo;
	delete[] peo_indices;
	delete[] h;
	delete[] a;
	delete[] b;

	return closed;
}


/**
 * expects the graph to be closed
 * returns a labeling with regards to which the graph is closed
**/
std::pair<unsigned *, unsigned *> Graph::gen_closed_labeling_pair() {
	if (order < 1)
		return std::pair<unsigned *, unsigned *>(0, 0);

	unsigned * peo = new unsigned[order];
	unsigned * peo_indices = new unsigned[order];

	if (order == 1)
	{
		peo[0] = 1;
		peo_indices[0] = 0;

		return std::pair<unsigned *, unsigned *>(peo, peo_indices);
	}

	if (order == 2)
	{
		peo[0] = 1;
		peo[1] = 2;
		peo_indices[0] = 0;
		peo_indices[1] = 1;

		return std::pair<unsigned *, unsigned *>(peo, peo_indices);
	}

	bool closed = false;
	unsigned * h = new unsigned[order];
	unsigned * a = new unsigned[order / 2];
	unsigned * b = new unsigned[order / 2];

	gen_initial_peo(peo, peo_indices, h, a, b);
	closed = is_closed_wrt_labeling(peo, peo_indices);

	if (!closed)
		closed = test_pe_orderings(peo, peo_indices, h, a, b, order / 2);

	if (!closed)
		closed = peo_switch(peo, peo_indices, h, a, b, order / 2 - 1);

	if (!closed)
		closed = test_pe_orderings(peo, peo_indices, h, a, b, order / 2);

	delete[] h;
	delete[] a;
	delete[] b;

	return std::pair<unsigned *, unsigned *>(peo, peo_indices);
}


unsigned * Graph::gen_closed_labeling() {
	std::pair<unsigned *, unsigned *> labeling_pair = gen_closed_labeling_pair();
	delete[] labeling_pair.first;
	return labeling_pair.second;
}
