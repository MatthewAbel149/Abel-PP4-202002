#pragma once
#include <vector>
#include <fstream>
#include <math.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include "DDSTextureLoader.h"
#include <wrl/client.h>
#pragma comment(lib, "d3d11.lib")


typedef struct _WVP_ {
	DirectX::XMFLOAT4X4 wMatrix;
	DirectX::XMFLOAT4X4 vMatrix;
	DirectX::XMFLOAT4X4 pMatrix;
} WVP;

typedef struct _OBJ_VERTEX_ {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 tex;
	DirectX::XMFLOAT3 nrm;
}OBJ_VERTEX;

typedef struct _MTL_DATA_
{

}MTL_DATA;

typedef struct _OBJ_DATA_
{
	std::vector < OBJ_VERTEX> vertexList;
	std::vector< unsigned int > indexList;
	char material [128] = "";

}OBJ_DATA;

typedef struct _MODEL_DATA_
{
	
	std::vector < OBJ_VERTEX > vertexList;
	std::vector< unsigned int > indexList;

	ID3D11ShaderResourceView* srvData	= nullptr;
	ID3D11Buffer* iBufferData			= nullptr;
	ID3D11Buffer* vBufferData			= nullptr;

}MODEL_DATA;


bool LoadOBJ(
	const char* filepath,
	OBJ_DATA* modelData);

bool LoadOBJ(
	const char* modelPath,
	//const char* materialPath, //for loading multitextured objects
	const wchar_t* texturePath,
	MODEL_DATA* modelData,

	CD3D11_BUFFER_DESC* bDesc,
	D3D11_SUBRESOURCE_DATA* subData,
	ID3D11Device* myDev );

bool ReadMTL(
	const char* file,
	MTL_DATA* materialData);

void DisplayModel(
	MODEL_DATA* modelData,
	ID3D11SamplerState* samplerState,
	ID3D11Buffer* cBuff,
	ID3D11DeviceContext* myCon,
	D3D11_MAPPED_SUBRESOURCE* gpuBuffer,
	ID3D11PixelShader* pShader,
	ID3D11VertexShader* vShader,
	ID3D11InputLayout* layout,
	WVP matrix
);