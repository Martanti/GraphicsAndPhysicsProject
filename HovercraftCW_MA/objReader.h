#pragma once

#include <fstream>
#include <string>
#include <iostream>

#define MAX_VERT 100000
struct VertexCoords {
	float x;
	float y;
	float z;
};

struct Face
{
	int id1;
	int id2;
	int id3;
};

struct VertexTexture
{
	float x;
	float y;
};

struct fVertex {
	float coords[3];
	float texCoords[2];
	float normals[3];
};

class objReader
{
public:
	int numPts;
	int numFaces;
	int num;
	int numNormals;
	VertexCoords vertex[5000];
	VertexCoords normal[5000];
	Face faces[5000];
	VertexTexture texCoord[5000];
	Face texfaces[5000];
	Face normalFaces[5000];

	std::string str;

	int numVertices;
	//float* vertices;
	float test[MAX_VERT];

	fVertex* v;  //use for buffer data

	objReader();
	~objReader();
	void loadVertex(char* filename);
	void LoadFace(char* filename);
	void LoadModel(char* filename);
	void CalNormal(void);
	void drawObj();
	void LoadTexCoord(char* filename);

	void LoadNormals(char* filename);
};

