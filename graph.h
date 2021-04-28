#pragma once

#include <qvector.h>
#include <set>

struct Edge
{
    int m_iWeight = -1;

	
};

struct Vertex
{
    Vertex(int v, std::set<int> a);
	
    int vertex_number;
    std::set<int> adjacents;
	
    std::set<Vertex*> m_edges;
};

inline Vertex::Vertex(int v, std::set<int> a)
	: vertex_number(v), adjacents(std::move(a))
{}

class Graph
{
public:
    Graph(std::vector<Vertex> v)
		: vecVertices(std::move(v))
	{}

    void AddNode(Vertex const& v);

private:
    std::vector<Vertex> vecVertices;
};