#pragma once
#include <vector>
#include <fstream>
#include <math.h>
#include <DirectXMath.h>



typedef struct _OBJ_VERTEX_ {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 tex;
	DirectX::XMFLOAT3 nrm;
}OBJ_VERTEX;

typedef struct _OBJ_DATA_
{
	std::vector < OBJ_VERTEX> vertexList;
	std::vector< unsigned int > indexList;
}OBJ_DATA;


bool LoadOBJ(
	const char* filepath,
	OBJ_DATA* modelData);