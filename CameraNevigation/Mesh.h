#ifndef __MESH_H__
#define __MESH_H__

#pragma once
#include "Face.h"
#include "Edge.h"

class Mesh
{
public:
	vector<Vertex*>	m_Vertices;
	vector<Face*>	m_Faces;
	vector<Edge*>	m_Edges;
public:
	Mesh();
	Mesh(char *file)
	{
		LoadObj(file);
	}
	~Mesh();
public:
	void	InitFlag(void);
	void	LoadObj(char *file);
	void	BuildAdjacency(void);
	void	ComputeNormal(void);
	void	ComputeEdges(void);
	void	MoveToCenter(Vec3<double> minPos, Vec3<double> maxPos, double scale = 1.0);
public:
	void	ComputeCotanWeight(void);
	void	Smoothing(int times = 1);
	void	LaplacianSmoothing(int times = 1);
	void	LaplacianSmoothingWithCotan(int times = 1);
	void	TaubinSmoothing(int times = 1); 
public:
	void	Draw(int type = 0);
	void	DrawSolid(void);
	void	DrawWire(void);	
};

#endif