#pragma once
#include <vector>
#include <fstream>
#include <math.h>
#include <DirectXMath.h>



typedef struct _OBJ_VERTEX_ {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;
	DirectX::XMFLOAT3 nrm;
}OBJ_VERTEX;

typedef struct _OBJ_DATA_ //needs capacity for multiple vertex/texture vectors
{
	std::vector < OBJ_VERTEX> vertexList;
	std::vector < wchar_t > textureList;

	std::vector< unsigned int> indexList;

}OBJ_DATA;


bool LoadOBJ(
	const char* filepath,
	std::vector < DirectX::XMFLOAT3 > &out_vertices,
	std::vector < DirectX::XMFLOAT2 > &out_uvs,
	std::vector < DirectX::XMFLOAT3 > &out_normals);