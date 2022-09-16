#ifndef __VERTEX_H__
#define __VERTEX_H__

#pragma once
#include <vector>
#include "Vec3.h"
using namespace std;

class Face;
class Edge;
class Vertex
{
public:
	int				m_Index;
	bool			m_Flag;
	Vec3<double>	m_Normal;
	Vec3<double>	m_Position;
	vector<Face*>	m_NbFaces;
	vector<Vertex*>	m_NbVertices;
	vector<Edge*>	m_NbEdges;
public:
	Vertex();
	Vertex(int index, Vec3<double> pos)
	{
		m_Flag = false;
		m_Index = index;
		m_Position = pos;
	}
	~Vertex();
public:
	inline double x(void) { return m_Position[0]; }
	inline double y(void) { return m_Position[1]; }
	inline double z(void) { return m_Position[2]; }
};

#endif