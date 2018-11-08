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
* Brian Kernighan's algorithm to count set bits in integer (counts ones in binary representation)
**/
inline unsigned count_set_bits(int n)
{
	unsigned count = 0;
	while (n)
	{
		n &= (n - 1);
		count++;
	}
	return count;
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
		if (!adjacent(pivot, v))
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
					if (adjacent(v, w))
						new_some.push_back(w);
				}

				if (j < none_size)
				{
					unsigned w = include_none->at(j);
					if (adjacent(v, w))
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
 * tests if a given subset in binary representation of order subset_order is an induced connected subgraph via depth-first-search
**/
bool Graph::is_induced_connected(unsigned * vertices, unsigned subset_order) {
	if (subset_order < 2)
		return true;

	bool * visited = new bool[subset_order];
	std::vector<unsigned> stack = { vertices[0] };

	visited[0] = true;
	for (unsigned i = 1; i < subset_order; i++)
		visited[i] = false;

	while (!stack.empty())
	{
		unsigned current = stack.back();
		stack.pop_back();

		for (unsigned i = 0; i < subset_order; i++)
		{
			unsigned neighbor = vertices[i];
			if (!visited[i]
				&& adjacent(current, neighbor))
			{
				stack.push_back(neighbor);
				visited[i] = true;
			}
		}
	}

	for (unsigned i = 1; i < subset_order; i++)
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
* tests if a given subset in binary representation of order subset_order is an induced path
* this is done by tallying up degrees, checking for invalid degrees, computing the check sum of degrees and checking if the induced subgraph is connected
**/
bool Graph::is_induced_path(int subset, unsigned subset_order) {
	unsigned * degrees = new unsigned[subset_order];
	unsigned * vertices = new unsigned[subset_order];

	for (unsigned i = 0, v = 1; i < subset_order; i++, v++)
	{
		degrees[i] = 0;
		while (!(subset & (1 << (order - v))))
			v++;
		vertices[i] = v;
	}

	for (unsigned i = 0; i < subset_order; i++)
	{
		for (unsigned j =  i + 1; j < subset_order; j++)
		{
			if (adjacent(vertices[i], vertices[j]))
			{
				degrees[i]++;
				degrees[j]++;
			}
		}
	}

	unsigned check_sum_degrees = 0;

	for (unsigned i = 0; i < subset_order; i++)
	{
		if (degrees[i] != 1
			&& degrees[i] != 2)
		{
				delete[] degrees;
				delete[] vertices;
				return false;
		}
		check_sum_degrees += degrees[i];
	}

	if (check_sum_degrees != (subset_order - 1) * 2)
	{
		delete[] vertices;
		delete[] degrees;
		return false;
	}

	bool connected = subset_order <= 4 || is_induced_connected(vertices, subset_order);

	delete[] vertices;
	delete[] degrees;

	return connected;
}


/**
* tests if a given order-4-subset of vertices is an induced claw
* this is done by tallying up degrees, checking for invalid degrees and computing the check sum of degrees
**/
bool Graph::is_induced_claw(int subset) {
	unsigned * degrees = new unsigned[4];
	unsigned * vertices = new unsigned[4];

	for (unsigned i = 0, v = 1; i < 4; i++, v++)
	{
		degrees[i] = 0;
		while (!(subset & (1 << (order - v))))
			v++;
		vertices[i] = v;
	}

	for (unsigned i = 0; i < 4; i++)
	{
		for (unsigned j = i + 1; j < 4; j++)
		{
			if (adjacent(vertices[i], vertices[j]))
			{
				degrees[i]++;
				degrees[j]++;
			}
		}
	}

	delete[] vertices;

	unsigned check_sum_degrees = 0;

	for (unsigned i = 0; i < 4; i++)
	{
		if (degrees[i] != 1
			&& degrees[i] != 3)
		{
			delete[] degrees;
			return false;
		}
		check_sum_degrees += degrees[i];
	}

	delete[] degrees;

	return check_sum_degrees == 6;
}


/**
* tests if given vertex is universal, i.e. it is adjacent to all other vertices of the graph
**/
bool Graph::is_universal(unsigned vertex) {
	for (unsigned w = 1; w <= order; w++)
		if (w != vertex
			&& !adjacent(vertex, w))
			return false;

	return true;
}


/**
* tests if the given vertex is simplicial, i.e., its neighbors form a clique, with respect to the subgraph induced by the vertices which have not been visited
**/
bool Graph::is_simplicial(unsigned vertex, bool * visited) {
	for (unsigned v = 1; v <= order; v++)
	{
		if (!visited[v - 1]
			&& adjacent(vertex, v))
		{
			for (unsigned w = v + 1; w <= order; w++)
			{
				if (!visited[w - 1]
					&& adjacent(vertex, w)
					&& !adjacent(v, w))
					return false;
			}
		}
	}

	return true;
}


/**
* returns a pair of simplicial vertices of the subgraph induced by the vertices which have not been visited
* expects the subgraph to have at least two simplicial vertices (this is the case, e.g., if the graph is chordal)
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
 * generates a lexicographic ordering on the vertices of the graph via lexicographic-breadth-first-search using partition refinement
**/
std::pair<unsigned *, unsigned *> Graph::gen_lexicographic_ordering() {
	if (order == 0)
		return {};

	std::list<std::vector<unsigned>> partitions;
	std::vector<unsigned> initial_list;

	for (unsigned v = 1; v <= order; v++)
		initial_list.push_back(v);

	partitions.push_back(initial_list);

	unsigned * ordering = new unsigned[order];
	unsigned * ordering_indices = new unsigned[order];

	for (int i = order - 1; i >= 0; i--)
	{
		unsigned v = partitions.back().back();
		partitions.back().pop_back();
		if (partitions.back().empty())
			partitions.pop_back();

		ordering[i] = v;
		ordering_indices[v - 1] = i;

		std::list<std::vector<unsigned>>::iterator it;
		for (it = partitions.begin(); it != partitions.end();)
		{
			std::vector<unsigned> intersection;
			std::vector<unsigned> difference;

			for (unsigned w : *it)
			{
				if (adjacent(v, w))
					intersection.push_back(w);
				else
					difference.push_back(w);
			}

			it = partitions.erase(it);
			if (!difference.empty())
				partitions.insert(it, difference);
			if (!intersection.empty())
				partitions.insert(it, intersection);
		}
	}

	return { ordering, ordering_indices };
}


/**
* tests if the vertices in given perfect elimination ordering at index t and t+1 are swappable, i.e. the peo with these vertices swapped is a peo as well
**/
bool Graph::peo_swappable(unsigned * peo, unsigned * h, int t) {
	if (t == ((int)order) - 1)
		return false;

	unsigned x = peo[t];
	unsigned y = peo[t + 1];

	return !adjacent(x, y) || h[x - 1] == h[y - 1] + 1;
}


/**
* swaps the vertices in given given perfect elimination ordering at index t and t+1 and tests if the graph is closed with respect to the induced labeling
**/
bool Graph::peo_move(unsigned * peo, unsigned * peo_indices, unsigned * h, int t) {
	unsigned x = peo[t];
	unsigned y = peo[t + 1];

	peo[t] = y;
	peo[t + 1] = x;
	unsigned temp = peo_indices[x - 1];
	peo_indices[x - 1] = peo_indices[y - 1];
	peo_indices[y - 1] = temp;

	if (adjacent(x, y))
	{
		h[x - 1]--;
		h[y - 1]++;
	}

	return is_closed_wrt_labeling(peo, peo_indices);
}


/**
* a special swap where the order of the t-th pair of simplicial vertices is swapped
* also tests if the graph is closed with respect to the new labeling
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

		if (adjacent(x, y))
		{
			h[x - 1]--;
			h[y - 1]++;
		}
	}

	return is_closed_wrt_labeling(peo, peo_indices);
}


/**
* recursively iterates through all perfect elimination orderings of the graph (twice) by swapping consecutive elements
* returns true as soon as an ordering is found for which the graph is closed with respect to the induced labeling
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
* generates an initial perfect elimination ordering via consecutive elimination of pairs of simplicial vertices
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
			if (adjacent(v, peo[i]))
				neighbour_count++;
		}
		h[v - 1] = neighbour_count;
	}

	delete[] visited;
}


//########## public member functions ##########
/**
 * constructor for given graph order and adjacency matrix
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
			if (adjacencies[get_index(v, w, order)])
				size++;
}


/**
 * expects a list of edges of the following form "{{1,2},{1,4},{2,3},{3,5}}"
 * changes 'this' to be the graph specified by order and edges
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
 * changes 'this' to be the graph specified by order and edges
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
* returns number of edges
**/
unsigned Graph::get_size() {
	return size;
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
			if (adjacent(first_vertex, second_vertex))
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



/**
* expects labeling to be a list of indices specifying the desired order of vertices
* converts the adjacency matrix into a string containing all edges in the form of "{{1,2},{1,4},{2,3},{3,5}}"
* the edges are given with respect to the given labeling
**/
std::string Graph::convert_to_string_wrt_labeling(unsigned * labeling) {
	std::string edges = "{";

	for (unsigned first_vertex = 1; first_vertex <= order; first_vertex++)
	{
		for (unsigned second_vertex = first_vertex + 1; second_vertex <= order; second_vertex++)
		{
			if (adjacent(first_vertex, second_vertex))
			{
				edges.push_back('{');
				edges += std::to_string(labeling[first_vertex - 1] + 1);
				edges.push_back(',');
				edges += std::to_string(labeling[second_vertex - 1] + 1);
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
 * converts the graph to a string using the Graph6 format
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
 * expects a file where graphs are parsed in one line in Graph6-format (e.g., a standard file generated by nauty)
 * changes 'this' to be the graph specified by read line
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
* expects a file where graphs are parsed in one line with vertex number and edge-list, e.g., 3 {1,2},{2,3}
* changes 'this' to be the graph specified by read line
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
 * returns the edgewise complement of the graph
**/
Graph Graph::get_complement() {
	unsigned * adj = new unsigned[order * order];

	for (unsigned v = 1; v <= order; v++)
	{
		for (unsigned w = v + 1; w <= order; w++)
		{
			unsigned i = get_index(v, w, order);
			unsigned j = get_index(w, v, order);

			adj[i] = 1 - adjacencies[i];
			adj[j] = 1 - adjacencies[j];
		}
	}

	return Graph(order, adj);
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
 * returns the detour number, i.e., the length of the longest induce path by iterating over all vertex subsets
 * this is done by taking advantage of the binary representation of integers (e.g., for order n=5: 6 = 1010 = {2,4})
**/
std::vector<unsigned> Graph::get_detour_number() {
	unsigned pow_set_size = nth_power(2, order);
	unsigned max_detour = 0;

	for (int subset = pow_set_size - 1; subset > 0; subset--)
	{
		unsigned subset_order = count_set_bits(subset);

		if (max_detour < subset_order - 1
			&& is_induced_path(subset, subset_order))
			max_detour = subset_order - 1;
	}

	return { max_detour };
}


/**
 * returns the minimum and maximum degree of vertices in the graph
**/
std::vector<unsigned> Graph::get_extreme_degrees() {
	unsigned max_deg = 0;
	unsigned min_deg = order - 1;

	for (unsigned v = 1; v <= order; v++)
	{
		unsigned degree = 0;
		
		for (unsigned w = 1; w <= order; w++)
			degree += adjacencies[get_index(v, w, order)];

		if (degree > max_deg)
			max_deg = degree;
		if (degree < min_deg)
			min_deg = degree;
	}

	return { min_deg, max_deg };
}


/**
* returns the independence number and the number of inclusion-maximal independent sets via respective clique numbers in the complement graph
**/
std::vector<unsigned> Graph::get_independence_numbers() {
	Graph complement = get_complement();
	if (order == 3 && adjacent(1, 3) && adjacent(2, 3) && !adjacent(1, 2))
		std::cout << "\n" << complement.get_order() << " " << complement.convert_to_string() << "\n" << complement.get_clique_numbers().at(0) << " " << complement.get_clique_numbers().at(1) << "\n";
	return complement.get_clique_numbers();
}


/**
 * returns the girth of the graph, i.e., the minimum length of a cycle (0 if there are none), via modified breadth-first-search
**/
std::vector<unsigned> Graph::get_girth() {
	unsigned girth = order + 1;
	bool * visited = new bool[order];
	unsigned * parent = new unsigned[order];
	unsigned * distance = new unsigned[order];

	for (unsigned v = 1; v <= order; v++)
	{
		std::vector<unsigned> queue;

		for (unsigned neighbor = 1; neighbor <= order; neighbor++)
		{
			if (neighbor == v)
			{
				visited[neighbor - 1] = true;
				parent[neighbor - 1] = 0;
				distance[neighbor - 1] = 0;
			}
			else if (adjacent(v, neighbor))
			{
				visited[neighbor - 1] = false;
				parent[neighbor - 1] = v;
				distance[neighbor - 1] = 1;
				queue.push_back(neighbor);
			}
			else
			{
				visited[neighbor - 1] = false;
				parent[neighbor - 1] = 0;
				distance[neighbor - 1] = order + 1;
			}
		}

		if (queue.size() < 2)
			continue;

		while (!queue.empty())
		{
			unsigned current = queue.front();
			queue.erase(queue.begin());
			visited[current - 1] = true;

			for (unsigned neighbor = 1; neighbor <= order; neighbor++)
			{
				if (neighbor != current
					&& adjacent(current, neighbor)
					&& neighbor != parent[current - 1])
				{
					if (!visited[neighbor - 1])
					{
						parent[neighbor - 1] = current;
						distance[neighbor - 1] = distance[current - 1] + 1;
						queue.push_back(neighbor);
					}
					else if (girth > distance[neighbor - 1] + distance[current - 1] + 1)
						girth = distance[neighbor - 1] + distance[current - 1] + 1;
				}
			}
		}
	}

	delete[] visited;
	delete[] parent;
	delete[] distance;

	if (girth == order + 1)
		girth = 0;
	return { girth };
}


/**
* tests if the graph is closed with respect to the labeling induced by given (perfect elimination) ordering, i.e.
* {a,b},{i,j} in E(G) with a < b, i < j  =>  {a,i} in E(G) if b=j and {b,j} in E(G) if a=i
**/
bool Graph::is_closed_wrt_labeling(unsigned * peo, unsigned * peo_indices) {
	for (unsigned vertex = 1; vertex <= order; vertex++)
	{
		int nearest_prior_neighbour;
		for (nearest_prior_neighbour = (int)(peo_indices[vertex - 1]) - 1; nearest_prior_neighbour >= 0; nearest_prior_neighbour--)
		{
			if (adjacent(vertex, peo[nearest_prior_neighbour]))
				break;
		}

		if (nearest_prior_neighbour >= 0)
		{
			for (int prior_neighbour = 0; prior_neighbour < nearest_prior_neighbour; prior_neighbour++)
			{
				if (adjacent(vertex, peo[prior_neighbour])
					&& !adjacent(peo[nearest_prior_neighbour], peo[prior_neighbour]))
					return false;
			}
		}

		int nearest_later_neighbour;
		for (nearest_later_neighbour = (int)(peo_indices[vertex - 1]) + 1; nearest_later_neighbour < (int)order; nearest_later_neighbour++)
		{
			if (adjacent(vertex, peo[nearest_later_neighbour]))
				break;
		}

		if (nearest_later_neighbour < (int)order)
		{
			for (int later_neighbour = order - 1; later_neighbour > nearest_later_neighbour; later_neighbour--)
			{
				if (adjacent(vertex, peo[later_neighbour])
					&& !adjacent(peo[nearest_later_neighbour], peo[later_neighbour]))
					return false;
			}
		}
	}

	return true;
}


/**
 * tests if the graph is connected
**/
bool Graph::is_connected() {
	unsigned * vertices = new unsigned[order];
	for (unsigned i = 0; i < order; i++)
		vertices[i] = i + 1;

	bool connected = is_induced_connected(vertices, order);

	delete[] vertices;
	return connected;
}


/**
 * tests if the graph is a cograph, i.e. it has no induced path of length 3
**/
bool Graph::is_cograph() {
	if (order < 4)
		return true;

	unsigned pow_set_size = nth_power(2, order);

	for (int subset = pow_set_size - 1; subset >= 15; subset--)
	{
		unsigned subset_order = count_set_bits(subset);

		if (subset_order == 4
			&& is_induced_path(subset, 4))
			return false;
	}

	return true;
}


/**
 * tests if the graph is euler, i.e. it has no vertices of odd degree
**/
bool Graph::is_euler() {
	if (order < 1)
		return true;

	unsigned * degrees = new unsigned[order];

	for (unsigned i = 0; i < order; i++)
		degrees[i] = 0;

	for (unsigned first_vertex = 1; first_vertex <= order; first_vertex++)
	{
		for (unsigned second_vertex = first_vertex + 1; second_vertex <= order; second_vertex++)
		{
			if (adjacent(first_vertex, second_vertex))
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

	std::pair<unsigned *, unsigned *> ordering = gen_lexicographic_ordering();

	for (unsigned v = 1; v <= order; v++)
	{
		unsigned parent_index = ordering.second[v - 1] + 1;
		while (parent_index < order)
		{
			if (adjacent(v, ordering.first[parent_index]))
				break;
			parent_index++;
		}

		if (parent_index >= order)
			continue;

		for (unsigned w_index = order - 1; w_index > parent_index; w_index--)
		{
			if (adjacent(v, ordering.first[w_index])
				&& !adjacent(ordering.first[parent_index], ordering.first[w_index]))
			{
				delete[] ordering.first;
				delete[] ordering.second;
				return false;
			}
		}
	}

	delete[] ordering.first;
	delete[] ordering.second;
	return true;
}


/**
 * tests if the graph is claw-free, i.e. it has no complete 1-3-bipartite induced subgraph
**/
bool Graph::is_clawfree() {
	if (order < 4)
		return true;

	unsigned pow_set_size = nth_power(2, order);

	for (int subset = pow_set_size - 1; subset >= 15; subset--)
	{
		unsigned subset_order = count_set_bits(subset);

		if (subset_order == 4
			&& is_induced_claw(subset))
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
 * tests if the graph is a cone, i.e. if it has a universal vertex
**/
bool Graph::is_cone() {
	for (unsigned v = 1; v <= order; v++)
	{
		if (is_universal(v))
			return true;
	}
	return false;
}


/**
 * expects the graph to be closed
 * returns a labeling with respect to which the graph is closed
**/
unsigned * Graph::gen_closed_labeling() {
	if (order < 1)
		return 0;

	unsigned * peo_indices = new unsigned[order];

	if (order == 1)
	{
		peo_indices[0] = 0;
		return peo_indices;
	}

	if (order == 2)
	{
		peo_indices[0] = 0;
		peo_indices[1] = 1;
		return peo_indices;
	}

	unsigned * peo = new unsigned[order];
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
		test_pe_orderings(peo, peo_indices, h, a, b, order / 2);

	delete[] h;
	delete[] a;
	delete[] b;
	delete[] peo;
	return peo_indices;
}
