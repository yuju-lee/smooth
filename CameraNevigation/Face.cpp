#include "Face.h"
#include "Edge.h"

Face::Face()
{
}


Face::~Face()
{
}

int	Face::GetIndex(Vertex *v)
{
	for (int i = 0; i < 3; i++) {
		if (m_Vertices[i] == v) {
			return i;
		}
	}
	return -1;
}

int	Face::HasVertex(Vertex *tv)
{
	for (auto v : m_Vertices) {
		if (v == tv) {
			return 1;
		}
	}
	return 0;
}

Vertex *Face::GetOtherVertex(Edge *edge)
{
	for (int i = 0; i < 3; i++) {
		auto v = m_Vertices[i];
		if (edge->m_Vertices[0] != v && edge->m_Vertices[1] != v) {
			return v;
		}
	}
	return nullptr;
}