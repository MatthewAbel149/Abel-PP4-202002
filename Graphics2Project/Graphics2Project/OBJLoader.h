#pragma once
#include <vector>
#include <fstream>
#include <math.h>



typedef struct _OBJ_VERTEX_
{
	float vertices[3];
	float uv[2];
	float normal[3];
	//XMFLOAT3 vertices;
	//XMFLOAT2 uvs;
	//XMFLOAT3 normals;
}OBJ_VERTEX;

typedef struct _OBJ_DATA_
{
	std::vector < OBJ_VERTEX> vertexList;
	wchar_t texture;
}OBJ_DATA;


bool LoadOBJ(const char* filepath, OBJ_DATA modelOutput);