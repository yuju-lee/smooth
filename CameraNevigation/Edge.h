#ifndef __EDGE_H__
#define __EDGE_H__

#pragma once
#include "Vertex.h"

class Edge
{
public:
	int				m_Index;
	vector<Vertex*>	m_Vertices;
	vector<Face*>	m_NbFaces;
public:
	double			m_CotanWeight;
public:
	Edge();
	Edge(int index, Vertex *v0, Vertex *v1)
	{
		m_Index = index;
		m_Vertices.push_back(v0);
		m_Vertices.push_back(v1);
		v0->m_NbEdges.push_back(this);
		v1->m_NbEdges.push_back(this);
	}
	~Edge();
public:

};

#endif