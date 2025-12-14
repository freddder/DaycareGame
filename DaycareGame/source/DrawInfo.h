#pragma once
#include "Hash.h"

#include <vector>

struct sVertexData
{
	float x, y, z, w;
	//float r, g, b, a;
	float nx, ny, nz, nw;
	float u1, v1, u2, v2;
};

struct sMeshDrawInfo
{
	unsigned int VAO_ID;
	unsigned int VBO_ID;
	unsigned int INDEX_ID;

	unsigned int numberOfVertices;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	Hash_v textureHash;
};

struct sModelDrawInfo
{
	unsigned int numMeshes;
	unsigned int totalNumOfVertices;

	std::vector<sMeshDrawInfo> allMeshesData;
};