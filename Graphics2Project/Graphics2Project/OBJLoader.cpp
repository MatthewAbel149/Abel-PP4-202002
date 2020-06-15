#include "OBJLoader.h"

bool LoadOBJ(
	const char* filepath,
	OBJ_DATA* modelData)
{
	FILE* file;
	errno_t err = fopen_s(&file, filepath, "r");

	if (file == NULL) {
		printf("Cannot open file!\n");
		return false;
	}

	OBJ_DATA modelDataOutput;

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;

	std::vector<DirectX::XMFLOAT3> vertex;
	std::vector<DirectX::XMFLOAT3> texels;
	std::vector<DirectX::XMFLOAT3> normal;

	std::vector<DirectX::XMFLOAT3> out_vertices;
	std::vector<DirectX::XMFLOAT3> out_uvs;
	std::vector<DirectX::XMFLOAT3> out_normals;
	char material[128] = "";


	while (true)
	{
		char lineheader[128] = "";

		// read the first word of a line and put it in lineheader
		int res = fscanf_s(file, "%s", lineheader, _countof(lineheader));

		if (res == EOF)
			break; //if the end of the file is reached, break the loop

		// else parse the line header
		if (strcmp(lineheader, "v") == 0)
		{
			DirectX::XMFLOAT3 temp_vertex = { 0,0,0 };
			fscanf_s(file, "%f %f %f\n", &temp_vertex.x, &temp_vertex.y, &temp_vertex.z);
			vertex.push_back(temp_vertex);
		}

		else if (strcmp(lineheader, "vt") == 0)
		{
			DirectX::XMFLOAT3 temp_texels = { 0,0,0 };
			fscanf_s(file, "%f %f %f\n", &temp_texels.x, &temp_texels.y, &temp_texels.z);
			//temp_texels.y = temp_texels.y * -1;
			temp_texels.y = 1 - temp_texels.y;
			texels.push_back(temp_texels);
		}

		else if (strcmp(lineheader, "vn") == 0)
		{
			DirectX::XMFLOAT3 temp_normal = { 0,0,0 };
			fscanf_s(file, "%f %f %f\n", &temp_normal.x, &temp_normal.y, &temp_normal.z);
			normal.push_back(temp_normal);
		}

		else if (strcmp(lineheader, "usemtl") == 0)
		{
			fscanf_s(file, "%s\n", &material, _countof(material));
		}

		else if (strcmp(lineheader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	//process vertex data
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		unsigned int vertexIndex = vertexIndices[i];
		DirectX::XMFLOAT3 tempVertex = vertex[vertexIndex - 1];
		out_vertices.push_back(tempVertex);
	}

	//process texture data
	for (unsigned int i = 0; i < uvIndices.size(); i++) {

		unsigned int uvIndex = uvIndices[i];
		DirectX::XMFLOAT3 tempUVS = texels[uvIndex - 1];
		out_uvs.push_back(tempUVS);
	}

	//process normal data
	for (unsigned int i = 0; i < normalIndices.size(); i++) {

		unsigned int normalIndex = normalIndices[i];
		DirectX::XMFLOAT3 tempNormal = normal[normalIndex - 1];
		out_normals.push_back(tempNormal);
	}


	for (unsigned int i = 0; i < out_vertices.size(); ++i)
	{
		OBJ_VERTEX tempVertexOutput;

		tempVertexOutput.pos = out_vertices[i];
		tempVertexOutput.tex = out_uvs[i];
		tempVertexOutput.nrm = out_normals[i];

		modelDataOutput.vertexList.push_back(tempVertexOutput);
	}

	for (unsigned int i = 0; i < modelDataOutput.vertexList.size(); ++i)
	{
		modelDataOutput.indexList.push_back(i);
	}

	*modelData = modelDataOutput;

	return true;
}


bool LoadOBJ(
	const char* modelPath,
	const wchar_t* texturePath,
	MODEL_DATA* modelData,
	CD3D11_BUFFER_DESC* bDesc,
	D3D11_SUBRESOURCE_DATA* subData,
	ID3D11Device* myDev)
{

	HRESULT hr;

	OBJ_DATA OBJData;
	if (LoadOBJ(modelPath, &OBJData))
	{

		modelData->vertexList = OBJData.vertexList;
		modelData->indexList = OBJData.indexList;


		bDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bDesc->ByteWidth = modelData->vertexList.size() * sizeof(OBJ_VERTEX);
		bDesc->CPUAccessFlags = 0;
		bDesc->MiscFlags = 0;
		bDesc->StructureByteStride = 0;
		bDesc->Usage = D3D11_USAGE_IMMUTABLE;
		subData->pSysMem = modelData->vertexList.data();

		hr = myDev->CreateBuffer(bDesc, subData, &(modelData->vBufferData));

		//index buffer mesh
		bDesc->BindFlags = D3D11_BIND_INDEX_BUFFER;
		bDesc->ByteWidth = modelData->indexList.size() * sizeof(unsigned int);
		subData->pSysMem = modelData->indexList.data();

		hr = myDev->CreateBuffer(bDesc, subData, &(modelData->iBufferData));

		hr = CreateDDSTextureFromFile(myDev, texturePath, nullptr, &(modelData->texture));

		return true;
	}
	else return false;
}


bool LoadModelFromOBJ(
	const wchar_t* texturePath,
	OBJ_DATA OBJData,
	MODEL_DATA* modelData,
	CD3D11_BUFFER_DESC* bDesc,
	D3D11_SUBRESOURCE_DATA* subData,
	ID3D11Device* myDev)
{
	HRESULT hr;


	modelData->vertexList = OBJData.vertexList;
	modelData->indexList = OBJData.indexList;


	bDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc->ByteWidth = modelData->vertexList.size() * sizeof(OBJ_VERTEX);
	bDesc->CPUAccessFlags = 0;
	bDesc->MiscFlags = 0;
	bDesc->StructureByteStride = 0;
	bDesc->Usage = D3D11_USAGE_IMMUTABLE;
	subData->pSysMem = modelData->vertexList.data();

	hr = myDev->CreateBuffer(bDesc, subData, &(modelData->vBufferData));

	//index buffer mesh
	bDesc->BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc->ByteWidth = modelData->indexList.size() * sizeof(unsigned int);
	subData->pSysMem = modelData->indexList.data();

	hr = myDev->CreateBuffer(bDesc, subData, &(modelData->iBufferData));

	hr = CreateDDSTextureFromFile(myDev, texturePath, nullptr, &(modelData->texture));

	return true;
}


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
)
{
	UINT mesh_strides[] = { sizeof(OBJ_VERTEX) };
	UINT mesh_offsets[] = { 0 };
	ID3D11Buffer* meshVB[] = { modelData->vBufferData };

	myCon->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
	myCon->IASetIndexBuffer(modelData->iBufferData, DXGI_FORMAT_R32_UINT, 0);

	//load the vertex shader for the mesh
	myCon->VSSetShader(vShader, 0, 0);
	//load the pixel shader for the mesh
	myCon->PSSetShader(pShader, 0, 0);
	//load the layout for the mesh
	myCon->IASetInputLayout(layout);



	myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, gpuBuffer);
	*((WVP*)(gpuBuffer->pData)) = matrix;
	myCon->Unmap(cBuff, 0);

	myCon->PSSetSamplers(0, 1, &samplerState);
	/*if (modelData->srvData)*/ myCon->PSSetShaderResources(0, 1, &(modelData->texture));
	myCon->DrawIndexed(modelData->indexList.size(), 0, 0);
}