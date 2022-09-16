#include "Mesh.h"
#include "GL\glut.h"
#include <algorithm>
#include "Common.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::LoadObj(char *file)
{
	FILE *fp;

	int index[3], id;
	char buffer[256] = { 0 };
	Vec3<double> pos, minBoundary, maxBoundary;
	minBoundary.Set(DBL_MAX);
	maxBoundary.Set(DBL_MIN);

	// Read vertex
	id = 0;
	fopen_s(&fp, file, "r");
	while (fscanf(fp, "%s %lf %lf %lf", &buffer, &pos[0], &pos[1], &pos[2]) != EOF) {
		if (buffer[0] == 'v' && buffer[1] == NULL) {
			if (minBoundary[0] > pos[0])	minBoundary[0] = pos[0];
			if (minBoundary[1] > pos[1])	minBoundary[1] = pos[1];
			if (minBoundary[2] > pos[2])	minBoundary[2] = pos[2];
			if (maxBoundary[0] < pos[0])	maxBoundary[0] = pos[0];
			if (maxBoundary[1] < pos[1])	maxBoundary[1] = pos[1];
			if (maxBoundary[2] < pos[2])	maxBoundary[2] = pos[2];
			m_Vertices.push_back(new Vertex(id++, pos));
		}
	}
	printf("Number of vertex : %d\n", m_Vertices.size());

	// Read face
	id = 0;
	fseek(fp, 0, SEEK_SET);
	while (fscanf(fp, "%s %d %d %d", &buffer, &index[0], &index[1], &index[2]) != EOF) {
		if (buffer[0] == 'f' && buffer[1] == NULL) {
			m_Faces.push_back(new Face(id++, m_Vertices[index[0] - 1], m_Vertices[index[1] - 1], m_Vertices[index[2] - 1]));
		}
	}
	printf("Number of face : %d\n", m_Faces.size());
	fclose(fp);

	MoveToCenter(minBoundary, maxBoundary, 5.0);
	BuildAdjacency();
	ComputeNormal();
	ComputeCotanWeight();
}

void Mesh::ComputeEdges(void)
{
	int id = 0;
	for (auto v : m_Vertices) {
		for (auto nv : v->m_NbVertices) {
			if (!nv->m_Flag) {
				m_Edges.push_back(new Edge(id++, v, nv));
			}
		}
		v->m_Flag = true;
	}
	printf("Number of edge : %d\n", m_Edges.size());
	InitFlag();
}

void Mesh::InitFlag(void)
{
	for (auto v : m_Vertices) {
		v->m_Flag = false;
	}
}

void Mesh::BuildAdjacency(void)
{
	// Clean
	for (auto v : m_Vertices) {
		v->m_NbVertices.clear();
		v->m_NbFaces.clear();
	}

	// Vertex and face
	for (auto f : m_Faces) {
		for (int j = 0; j < 3; j++) {
			Vertex *v = f->m_Vertices[j];
			v->m_NbFaces.push_back(f);
		}		
	}
	
	// Vertex and vertex
	for (auto v : m_Vertices) {
		for (auto nf : v->m_NbFaces) {
			int index = nf->GetIndex(v);		
			Add(v->m_NbVertices, nf->m_Vertices[(index + 1) % 3]);
			Add(v->m_NbVertices, nf->m_Vertices[(index + 3 - 1) % 3]);
		}
	}

	ComputeEdges();

	// Edge and face
	for (auto e : m_Edges) {
		auto v = e->m_Vertices[0];
		for (auto nf : v->m_NbFaces) {
			if (nf->HasVertex(e->m_Vertices[0]) && nf->HasVertex(e->m_Vertices[1])) {
				if (!Has(nf->m_NbEdges, e)) {
					nf->m_NbEdges.push_back(e);
				}
				if (!Has(e->m_NbFaces, nf)) {
					e->m_NbFaces.push_back(nf);
				}
			}
		}
	}
}

void Mesh::ComputeNormal(void)
{
	// Face normal
	for (auto f : m_Faces) {
		Vec3<double> a = f->m_Vertices[1]->m_Position - f->m_Vertices[0]->m_Position;
		Vec3<double> b = f->m_Vertices[2]->m_Position - f->m_Vertices[0]->m_Position;
		f->m_Normal = a.Cross(b);
		f->m_Normal.Normalize();
	}

	// Vertex normal
	for (auto v : m_Vertices) {
		int numNeighborFaces = (int)v->m_NbFaces.size();
		for (auto nf : v->m_NbFaces) {
			v->m_Normal += nf->m_Normal;
		}
		v->m_Normal /= numNeighborFaces;
	}
}

void Mesh::MoveToCenter(Vec3<double> minPos, Vec3<double> maxPos, double scale)
{
	double longestAxis = fmax(fmax(fabs(maxPos.x() - minPos.x()), fabs(maxPos.y() - minPos.y())), fabs(maxPos.z() - minPos.z()));
	Vec3<double> center = (maxPos + minPos) / 2.0;
	Vec3<double> position;
	Vec3<double> vector;
	Vec3<double> newCenter(0.0, 0.0, 0.0);

	for (auto v : m_Vertices) {
		position = v->m_Position;
		vector = position - center;
		vector /= longestAxis;
		vector *= scale;
		position = newCenter;
		position += vector;
		v->m_Position = position;
	}
}

void Mesh::DrawSolid(void)
{
	glPushMatrix();
	for (auto f : m_Faces) {
		Vec3<double> p[3], n[3];
		for (int j = 0; j < 3; j++) {
			p[j] = f->m_Vertices[j]->m_Position;
			n[j] = f->m_Vertices[j]->m_Normal;
		}
		glBegin(GL_POLYGON);
		glNormal3f((GLfloat)f->m_Normal.x(), f->m_Normal.y(), f->m_Normal.z());
		for (int j = 0; j < 3; j++) {
			//glNormal3f((GLfloat)n[j].x(), (GLfloat)n[j].y(), (GLfloat)n[j].z());
			glVertex3f((GLfloat)p[j].x(), (GLfloat)p[j].y(), (GLfloat)p[j].z());
		}
		glEnd();
	}
	glPopMatrix();
}

void Mesh::DrawWire(void)
{
	glPushMatrix();
	glLineWidth(1.0f);
	//glDisable(GL_LIGHTING);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (auto f : m_Faces) {
		Vec3<double> p[3], n[3];
		for (int j = 0; j < 3; j++) {
			p[j] = f->m_Vertices[j]->m_Position;
			n[j] = f->m_Vertices[j]->m_Normal;
		}
		glBegin(GL_LINES);
		for (int j = 0; j < 3; j++) {
			glNormal3f((GLfloat)n[j % 3].x(), (GLfloat)n[j % 3].y(), (GLfloat)n[j % 3].z());
			glVertex3f((GLfloat)p[j % 3].x(), (GLfloat)p[j % 3].y(), (GLfloat)p[j % 3].z());
			glNormal3f((GLfloat)n[(j + 1) % 3].x(), (GLfloat)n[(j + 1) % 3].y(), (GLfloat)n[(j + 1) % 3].z());
			glVertex3f((GLfloat)p[(j + 1) % 3].x(), (GLfloat)p[(j + 1) % 3].y(), (GLfloat)p[(j + 1) % 3].z());
		}
		glEnd();
	}
	glLineWidth(1.0f);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void Mesh::Draw(int type)
{
	if (type == 0) {
		//DrawWire();
		//glEnable(GL_POLYGON_OFFSET_LINE);
		//glPolygonOffset(1.0, 1.0);
		DrawSolid(); 
	} else if (type == 1) {
		DrawWire();
	}
}

void Mesh::TaubinSmoothing(int times)
{
	double lambda = 0.5;
	double mu = -0.67;
	int numVertex = m_Vertices.size();

	for (int t = 0; t < times; t++) {
		for (int i = 0; i < numVertex; i++) {
			auto v = m_Vertices[i];
			auto pos = v->m_Position;
			Vec3<double> sumPos;
			for (auto nv : v->m_NbVertices) {
				sumPos += nv->m_Position;
			}
			double valence = (double)v->m_NbVertices.size();
			auto finalPos = (sumPos / valence - pos) * lambda + pos;
			finalPos = (sumPos / valence - finalPos) * mu + finalPos;
			m_Vertices[i]->m_Position = finalPos;
		}
	}
	ComputeNormal();
}

void Mesh::LaplacianSmoothingWithCotan(int times)
{
	double lambda = 0.25;
	int numVertex = m_Vertices.size();

	for (int t = 0; t < times; t++) {
		for (int i = 0; i < numVertex; i++) {
			auto v = m_Vertices[i];
			Vec3<double> sumPos;
			double valence = 0.0;
			for (auto ne : v->m_NbEdges) {
				auto pos0 = ne->m_Vertices[0]->m_Position;
				auto pos1 = ne->m_Vertices[1]->m_Position;
				double weight = ne->m_CotanWeight;
				if (v == ne->m_Vertices[0]) {
					sumPos += (pos1 - pos0) * weight;
				} else {
					sumPos += (pos0 - pos1) * weight;
				}
				valence += weight;
			}
			auto finalPos = (sumPos / valence) * lambda + v->m_Position;
			m_Vertices[i]->m_Position = finalPos;
		}
	}
	ComputeNormal();
}

void Mesh::LaplacianSmoothing(int times)
{
	double lambda = 0.25;
	int numVertex = m_Vertices.size();

	for (int t = 0; t < times; t++) {
		for (int i = 0; i < numVertex; i++) {
			auto v = m_Vertices[i];
			auto pos = v->m_Position;
			Vec3<double> sumPos;
			for (auto nv : v->m_NbVertices) {
				sumPos += nv->m_Position;
			}
			double valence = (double)v->m_NbVertices.size();
			auto finalPos = (sumPos / valence - pos) * lambda + pos;
			m_Vertices[i]->m_Position = finalPos;
		}
	}
	ComputeNormal(); 
}

void Mesh::Smoothing(int times)
{
	int numVertex = m_Vertices.size();

	for (int t = 0; t < times; t++) {
		for (int i = 0; i < numVertex; i++) {
			auto v = m_Vertices[i];
			auto pos = v->m_Position;
			for (auto nv : v->m_NbVertices) {
				pos += nv->m_Position;
			}
			auto numConnections = v->m_NbVertices.size();
			numConnections++; // add : self vertex
			pos /= (double)numConnections;
			m_Vertices[i]->m_Position = pos;
		}
	}
	ComputeNormal();
}

//#define COTANGENT_TYPE_A // Cot(x) = 1 / tan(x)
#define COTANGENT_TYPE_B // Cot(x) = cos(x) / sin(x)

void Mesh::ComputeCotanWeight(void)
{
	for (auto e : m_Edges) {
		if (e->m_NbFaces.size() == 2) {
			double weight = 0.0;
			
			Vertex *other_v = e->m_NbFaces[0]->GetOtherVertex(e);
			Vec3<double> dir0 = e->m_Vertices[0]->m_Position - other_v->m_Position;
			Vec3<double> dir1 = e->m_Vertices[1]->m_Position - other_v->m_Position;
			dir0.Normalize();
			dir1.Normalize();
			
#ifdef COTANGENT_TYPE_A
			auto angle = acos(dir0.Dot(dir1));
			weight += 1.0 / tan(angle);
#endif
					   			
#ifdef COTANGENT_TYPE_B
			double sin_x = dir0.Cross(dir1).Length();
			double cos_x = dir0.Dot(dir1);
			weight += cos_x / sin_x;
#endif

			other_v = e->m_NbFaces[1]->GetOtherVertex(e);
			dir0 = e->m_Vertices[0]->m_Position - other_v->m_Position;
			dir1 = e->m_Vertices[1]->m_Position - other_v->m_Position;
			dir0.Normalize();
			dir1.Normalize();
			
#ifdef COTANGENT_TYPE_A
			auto angle = acos(dir0.Dot(dir1));
			weight += 1.0 / tan(angle);
#endif

#ifdef COTANGENT_TYPE_B
			sin_x = dir0.Cross(dir1).Length();
			cos_x = dir0.Dot(dir1);
			weight += cos_x / sin_x;
#endif

			e->m_CotanWeight = weight;
		} 
	}
}