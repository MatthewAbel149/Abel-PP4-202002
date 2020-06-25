// Graphics2Project.cpp : Defines the entry point for the application.
//
#include "Includes.h"
//void LoadModel(CD3D11_BUFFER_DESC&, D3D11_SUBRESOURCE_DATA&, const unsigned int**, const OBJ_VERT**);

#define NUM_LIGHTS 2

//#define SAFEDELETE(*ptr) if(ptr) { ptr. }

bool MakeGrid(int halfSize, float spaceBetweenLines, OBJ_DATA* ObjData, OBJ_VERTEX tempVertData);


using namespace DirectX;
using namespace std;

unsigned int numVerts;

// Math Stuff
WVP myMatrices;

typedef struct _cBuffData_ {
	Light lightVar[NUM_LIGHTS];

	XMFLOAT3 position;
	float timer;
} cBuffData;

////////////////////////////////////////
//	GLOBAL VARIABLE DECLARATIONS  
////////////////////////////////////////
// for init
ID3D11Device* myDev;
IDXGISwapChain* mySwap;
ID3D11DeviceContext* myCon;

// for drawing
ID3D11RenderTargetView* myRtv;
D3D11_VIEWPORT myPort;
float aspectRatio = 1;


//ID3D11Buffer* cBuff; //shader variables
vector<ID3D11Buffer*> cBuffList;


// Complex mesh variables
ID3D11PixelShader* pMeshShader;
ID3D11PixelShader* pLightShader;
ID3D11PixelShader* pColorShader;
ID3D11PixelShader* pReflectionShader;
ID3D11PixelShader* pSunShader;

ID3D11VertexShader* vMeshShader;
ID3D11VertexShader* vLightShader;
ID3D11VertexShader* vColorShader;
ID3D11VertexShader* vReflectionShader;
ID3D11VertexShader* vTextureToGridShader;

ID3D11GeometryShader* gShader;

ID3D11InputLayout* layoutVert;
vector< MODEL_DATA > modelList;
ID3D11SamplerState* samplerState;
D3D11_SAMPLER_DESC   samplerDesc;

vector<ID3D11ShaderResourceView*> skyboxTextures; // Left Front Right Back Top Bottom
MODEL_DATA skybox;
ID3D11PixelShader* pSkyboxShader;
ID3D11VertexShader* vSkyboxShader;


// Z buffer
ID3D11Texture2D* zBuffer;
ID3D11DepthStencilView* zBufferView;

HRESULT hr;

XMMATRIX camera = XMMatrixMultiply(XMMatrixTranslation(0, 6, -15), XMMatrixIdentity());

XTime timer;


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#pragma region Stuff I dont understand
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_GRAPHICS2PROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICS2PROJECT));

	MSG msg;

#pragma endregion


	int scene = 0;
	float color[] = { .0f, .0f, .8f, 1.f }; //color of clear buffer


	D3D11_MAPPED_SUBRESOURCE gpuBuffer;
	ID3D11Buffer* constants[] = { cBuffList[0] };

	//LPPOINT cursorPoint = nullptr;
	POINT cursorPoint;
	GetCursorPos(&cursorPoint);
	POINT originPoint = cursorPoint;
	POINT prevPoint;
	float mouseDeltaX, mouseDeltaY;

	cBuffData bufferData;
	bufferData.position = XMFLOAT3(0, 0, 0);
	bufferData.timer = 0;

	bufferData.lightVar[0].position = XMFLOAT3(0, 0, 0);
	bufferData.lightVar[0].direction = XMFLOAT3(0, 0, 0);
	bufferData.lightVar[0].color = XMFLOAT3(0, 0, 0);
	bufferData.lightVar[0].range = 10;
	bufferData.lightVar[0].intensity = 10;
	bufferData.lightVar[0].attenuation = 10;
	
	bufferData.lightVar[1].position = XMFLOAT3(0, 0, 0);
	bufferData.lightVar[1].direction = XMFLOAT3(0, 0, 0);
	bufferData.lightVar[1].color = XMFLOAT3(0, 0, 0);
	bufferData.lightVar[1].range = 10;
	bufferData.lightVar[1].intensity = 10;
	bufferData.lightVar[1].attenuation = 10;
	

	bool showCursor = false;
	bool showCursorPrev = true;


	/*
	static OBJ_VERTEX pointArr[14];
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			pointArr[i * 4 + j].pos = { i * 1.0f, 0, j * 1.0f };
			pointArr[i * 4 + j].nrm = { 0, 1, 0 };
			pointArr[i * 4 + j].tex = { 1, 1, 0 };
		}
	}
	*/

	float rot = 0;


	// Main message loop:
	while (true) //GetMessage(&msg, nullptr, 0, 0))
	{

#pragma region CheckForClose
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;
#pragma endregion

#pragma region camera cursor interaction


		if (showCursor ^ showCursorPrev)
			ShowCursor(showCursor);

		showCursorPrev = showCursor;

		if (GetAsyncKeyState(VK_ESCAPE) & 1)
			showCursor = !showCursor;



		prevPoint = originPoint;

		if (showCursor)
			prevPoint = cursorPoint;

		if (GetCursorPos(&cursorPoint))
		{
			mouseDeltaX = cursorPoint.x - prevPoint.x;
			mouseDeltaY = cursorPoint.y - prevPoint.y;

			mouseDeltaX = mouseDeltaX / 1920;
			mouseDeltaY = mouseDeltaY / 1080;
		}

		if (!showCursor)
			SetCursorPos(originPoint.x, originPoint.y);


		//SetCursorPos(prevPoint.x, prevPoint.y);



#pragma endregion


		myCon->ClearRenderTargetView(myRtv, color); //clear buffer

		myCon->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);
		// Set up the pipeline

		// Output Merger
		ID3D11RenderTargetView* tempRTV[] = { myRtv }; //to remove, pass nullptr instead of myRtv
		myCon->OMSetRenderTargets(1, tempRTV, zBufferView);
		// Rasterizer
		myCon->RSSetViewports(1, &myPort);


		rot += timer.Delta();

#pragma region camera


		XMMATRIX temporaryMatrix = XMMatrixIdentity();

		/////////////////////////////////////
		//	CAMERA CONTROLS
		/////////////////////////////////////

		XMMATRIX yRotMatrix = XMMatrixIdentity();

		float camSpeed = 5;
		if (GetAsyncKeyState(VK_LSHIFT)) camSpeed = 15; // SPEED UP


		yRotMatrix = XMMatrixRotationY(mouseDeltaX * 5);
		temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixRotationX(mouseDeltaY * 5));


		/////////////////////////////////////
		// CAMERA WASD CONTROLS
		/////////////////////////////////////
		if (GetAsyncKeyState('W')) //forward
		{
			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixTranslation(0, 0, timer.Delta() * camSpeed));
		}
		if (GetAsyncKeyState('S')) //back
		{
			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixTranslation(0, 0, -timer.Delta() * camSpeed));
		}
		if (GetAsyncKeyState('A')) //left
		{
			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixTranslation(-timer.Delta() * camSpeed, 0, 0));
		}
		if (GetAsyncKeyState('D')) //right
		{
			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixTranslation(timer.Delta() * camSpeed, 0, 0));
		}
		////////////////////////////////////////////////////////////////////////////

		camera = XMMatrixMultiply(temporaryMatrix, camera);

		temporaryMatrix = XMMatrixIdentity();

		////////////////////////////////////////////////////////////////////////////

		if (GetAsyncKeyState(VK_SPACE)) //up
		{
			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixTranslation(0, timer.Delta() * camSpeed, 0));
		}
		if (GetAsyncKeyState('C')) //down
		{
			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixTranslation(0, -timer.Delta() * camSpeed, 0));
		}

		////////////////////////////////////////////////////////////////////////////

		if (GetAsyncKeyState('R') & 1) // RESET CAMERA POSITION
		{
			camera = XMMatrixMultiply(XMMatrixTranslation(0, 6, -15), XMMatrixIdentity());

			temporaryMatrix = XMMatrixIdentity();

		}


		XMVECTOR cameraVector = camera.r[3];


		camera = XMMatrixMultiply(camera, yRotMatrix);
		camera.r[3] = cameraVector;

		camera = XMMatrixMultiply(camera, temporaryMatrix);


		temporaryMatrix = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, 0.1f, 1000);
		XMStoreFloat4x4(&myMatrices.pMatrix, temporaryMatrix); // move to projection space

		XMStoreFloat4x4(&myMatrices.vMatrix, XMMatrixInverse(0, camera)); //move to view space

#pragma endregion


		myCon->VSSetConstantBuffers(0, 1, constants);

		if (GetAsyncKeyState('N') & 1) scene += 1;
		if (scene == 3) scene -= 3;


#pragma region skybox
		{
			////////////////////////////////////////////////////////////////////////
			// SKYBOX RENDER
			////////////////////////////////////////////////////////////////////////

			myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			XMStoreFloat4x4(&myMatrices.wMatrix, XMMatrixTranslation(
				XMVectorGetX(camera.r[3]),
				XMVectorGetY(camera.r[3]),
				XMVectorGetZ(camera.r[3])
			));

			UINT mesh_strides[] = { sizeof(OBJ_VERTEX) };
			UINT mesh_offsets[] = { 0 };
			ID3D11Buffer* meshVB[] = { skybox.vBufferData };

			myCon->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
			myCon->IASetIndexBuffer(skybox.iBufferData, DXGI_FORMAT_R32_UINT, 0);

			//load the vertex shader for the mesh
			myCon->VSSetShader(vSkyboxShader, 0, 0);
			//load the pixel shader for the mesh
			myCon->PSSetShader(pSkyboxShader, 0, 0);
			//load the layout for the mesh
			myCon->IASetInputLayout(layoutVert);

			myCon->Map(cBuffList[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)((&gpuBuffer)->pData)) = myMatrices;
			myCon->Unmap(cBuffList[0], 0);

			myCon->PSSetSamplers(0, 1, &samplerState);
			myCon->PSSetShaderResources(0, 1, &skybox.texture);

			myCon->DrawIndexed(skybox.indexList.size(), 0, 0);
			myCon->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);
		}
#pragma endregion


		if (scene == 0) {

			skybox.texture = skyboxTextures[3];

			//bufferData.lightVar[0].position = XMFLOAT3(cos(rot) * 12, 3.f, sin(rot) * 12);
			bufferData.lightVar[0].position = XMFLOAT3(11, 3.f, 11);
			bufferData.lightVar[0].direction = XMFLOAT3(0.f, 0.f, 0.f);
			bufferData.lightVar[0].color = XMFLOAT3(1.f, .3f, .3f);
			bufferData.lightVar[0].attenuation = 1.0f;
			bufferData.lightVar[0].range = 10.0f;
			bufferData.lightVar[0].intensity = 1.0f;

			bufferData.lightVar[1].position = XMFLOAT3(0, 0, 0);
			bufferData.lightVar[1].direction = XMFLOAT3(-0.2f, -0.6f, -0.2f);
			bufferData.lightVar[1].color = XMFLOAT3(.7f, .9f, 1.f);
			bufferData.lightVar[1].range = 10.0f;
			bufferData.lightVar[1].intensity = 10;
			bufferData.lightVar[1].attenuation = 10;


#pragma region Stonehenge
			{

				XMStoreFloat4x4(&myMatrices.wMatrix, XMMatrixIdentity());

				myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


				/*
				//////////////////////////////////////////////
				// FlipFlop()
				//////////////////////////////////////////////
				static bool up = false;
				static float lightrot = 0;

				if (up) lightrot += timer.Delta();
				else	lightrot -= timer.Delta();

				if (lightrot >= 1) up = false;
				if (lightrot <= -1) up = true;
				//////////////////////////////////////////////
				*/


				myCon->PSSetConstantBuffers(0, 1, &cBuffList[1]);

				XMStoreFloat3(&bufferData.position, camera.r[3]);
				bufferData.timer = 0.1f;

				///////////////////////////////////////////////////////////////////////////////


				hr = myCon->Map(cBuffList[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
				*((cBuffData*)((&gpuBuffer)->pData)) = bufferData;
				myCon->Unmap(cBuffList[1], 0);


				DisplayModel(
					&modelList[0],
					samplerState,
					cBuffList[0],
					myCon,
					&gpuBuffer,
					//pMeshShader,
					//vMeshShader,
					pLightShader,
					vLightShader,
					layoutVert,
					myMatrices);
			}
#pragma endregion
#pragma region King
			{
				//-----------------------------------------
				//   matrix math
				//-----------------------------------------
				static float burgerRot = 0;

				if (GetAsyncKeyState('K'))
				{
					burgerRot += .1f;
				}

				temporaryMatrix = XMMatrixIdentity();

				temporaryMatrix = XMMatrixMultiply(XMMatrixTranslation(6, 1, 6), temporaryMatrix);
				temporaryMatrix = XMMatrixMultiply(XMMatrixScaling(.2f, .2f, .2f), temporaryMatrix);



				temporaryMatrix = XMMatrixMultiply(XMMatrixRotationY(burgerRot + 6.283), temporaryMatrix);

				XMStoreFloat4x4(&myMatrices.wMatrix, temporaryMatrix);
				//-----------------------------------------
				DisplayModel(
					&modelList[2],
					samplerState,
					cBuffList[0],
					myCon,
					&gpuBuffer,
					pMeshShader,
					vMeshShader,
					layoutVert,
					myMatrices);
			}
#pragma endregion
#pragma region Trash Can
	
			myCon->VSSetConstantBuffers(0, 1, constants);

			//matrix math
			temporaryMatrix = XMMatrixIdentity();

			temporaryMatrix = XMMatrixMultiply(XMMatrixTranslation(6, 1.6, 6), temporaryMatrix);
			temporaryMatrix = XMMatrixMultiply(XMMatrixRotationX(-3.14159265 / 2), temporaryMatrix);

			XMStoreFloat4x4(&myMatrices.wMatrix, temporaryMatrix);

			myCon->PSSetConstantBuffers(0, 1, &cBuffList[1]);


			XMStoreFloat3(&bufferData.position, cameraVector);
			bufferData.timer = 0.25f;

			//////////////////////////////////////////////
			// FLIP FLOP
			//////////////////////////////////////////////
			//static bool up = false;
			//static float scaleValue = 0;
			//
			//if (up) scaleValue += timer.Delta() / 1.2f;
			//else	scaleValue -= timer.Delta() / 1.2f;
			//
			//if (scaleValue >= 1) up = false;
			//if (scaleValue <= 0) up = true;
			//////////////////////////////////////////////
			//
			//bufferData.position = XMFLOAT3(myMatrices.wMatrix.m[0][2], myMatrices.wMatrix.m[1][2], myMatrices.wMatrix.m[2][2]);
			//bufferData.timer = timer.TotalTime();
			//
			//temporaryMatrix = XMMatrixMultiply(
			//	XMMatrixScaling(
			//		cos(scaleValue) + 0.22f,
			//		cos(scaleValue) + 0.22f,
			//		sin(scaleValue) + 0.75f),
			//	temporaryMatrix);
			//
			//bufferData.timer = 0.0f;
			//bufferData.timer = 1.0f;
			//

			hr = myCon->Map(cBuffList[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((cBuffData*)((&gpuBuffer)->pData)) = bufferData;
			myCon->Unmap(cBuffList[1], 0);

			myCon->PSSetShaderResources(1, 1, &skyboxTextures[3]);

			myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			DisplayModel(
				&modelList[5],
				samplerState,
				cBuffList[0],
				myCon,
				&gpuBuffer,
				//pColorShader,
				//vColorShader,
				pReflectionShader,
				vReflectionShader,
				//pLightShader,
				//vLightShader,
				layoutVert,
				myMatrices);

#pragma endregion
		}


		if (scene == 1) {
			skybox.texture = skyboxTextures[1];
#pragma region Pineapple
			{
				//*------------------- Load complex mesh
				//matrix math
				temporaryMatrix = XMMatrixIdentity();

				temporaryMatrix = XMMatrixMultiply(XMMatrixTranslation(-19, 6, -2), temporaryMatrix);

				temporaryMatrix = XMMatrixMultiply(XMMatrixScaling(.025f, .025f, .025f), temporaryMatrix);

				temporaryMatrix = XMMatrixMultiply(XMMatrixRotationZ(-0.05), temporaryMatrix);
				temporaryMatrix = XMMatrixMultiply(XMMatrixRotationY(0), temporaryMatrix);
				temporaryMatrix = XMMatrixMultiply(XMMatrixRotationX(0.1), temporaryMatrix);

				XMStoreFloat4x4(&myMatrices.wMatrix, temporaryMatrix);


				myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				DisplayModel(
					&modelList[1],
					samplerState,
					cBuffList[0],
					myCon,
					&gpuBuffer,
					pMeshShader,
					vMeshShader,
					layoutVert,
					myMatrices);
			}
#pragma endregion
#pragma region Palm tree
			{
				//*------------------- Load complex mesh
				//matrix math
				temporaryMatrix = XMMatrixIdentity();

				temporaryMatrix = XMMatrixMultiply(XMMatrixTranslation(-18, 4.75, -1), temporaryMatrix);

				temporaryMatrix = XMMatrixMultiply(XMMatrixScaling(4.0f, 4.0f, 4.0f), temporaryMatrix);


				XMStoreFloat4x4(&myMatrices.wMatrix, temporaryMatrix);


				myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				DisplayModel(
					&modelList[6],
					samplerState,
					cBuffList[0],
					myCon,
					&gpuBuffer,
					pMeshShader,
					vMeshShader,
					layoutVert,
					myMatrices);
			}
#pragma endregion
#pragma region Grid
			{
				//*------------------- Load complex mesh
				//matrix math
				temporaryMatrix = XMMatrixIdentity();

				XMStoreFloat4x4(&myMatrices.wMatrix, temporaryMatrix);

				myCon->VSSetSamplers(0, 1, &samplerState);
				myCon->VSSetShaderResources(0, 1, &(modelList[4].heightMap));

				myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				//myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


				DisplayModel(
					&modelList[4],
					samplerState,
					cBuffList[0],
					myCon,
					&gpuBuffer,
					pMeshShader,
					vTextureToGridShader,
					layoutVert,
					myMatrices);
				{ //future geometry shader basis
					//points
					//pointArr;


					//UINT mesh_strides[] = { sizeof(OBJ_VERTEX) };
					//UINT mesh_offsets[] = { 0 };
					//ID3D11Buffer* meshVB[] = { pointArr };

					//myCon->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
					//myCon->IASetIndexBuffer(modelData->iBufferData, DXGI_FORMAT_R32_UINT, 0);

					////load the vertex shader for the mesh
					//myCon->VSSetShader(vShader, 0, 0);
					////load the pixel shader for the mesh
					//myCon->PSSetShader(pShader, 0, 0);
					////load the layout for the mesh
					//myCon->IASetInputLayout(layout);



					//myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, gpuBuffer);
					//*((WVP*)(gpuBuffer->pData)) = matrix;
					//myCon->Unmap(cBuff, 0);

					//myCon->PSSetSamplers(0, 1, &samplerState);
					///*if (modelData->srvData)*/ myCon->PSSetShaderResources(0, 1, &(modelData->texture));
					//myCon->DrawIndexed(modelData->indexList.size(), 0, 0);
					////myCon->GSSetShader(gShader, 0, 0);
					////myCon->GSSetConstantBuffers(0, 0, &cBuffList[0]);



					////myCon->GSSetShader(nullptr, 0, 0);

				}
			}
#pragma endregion
#pragma region Water
			//matrix math
			temporaryMatrix = XMMatrixIdentity();

			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixTranslation(0, 2.2, 0));

			XMStoreFloat4x4(&myMatrices.wMatrix, temporaryMatrix);

			myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


			///////////////////////////////////////////////////////////////////////////////

			XMStoreFloat3(&bufferData.position, cameraVector);

			bufferData.timer = rot;
			//bufferData.timer = timer.TotalTime();

			hr = myCon->Map(cBuffList[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((cBuffData*)((&gpuBuffer)->pData)) = bufferData;
			myCon->Unmap(cBuffList[1], 0);
			///////////////////////////////////////////////////////////////////////////////


			DisplayModel(
				&modelList[9],
				samplerState,
				cBuffList[0],
				myCon,
				&gpuBuffer,
				pSunShader,
				vMeshShader,
				layoutVert,
				myMatrices);

#pragma endregion

		}


		if (scene == 2) {
			skybox.texture = skyboxTextures[0];

			bufferData.lightVar[0].position = XMFLOAT3(1.0f, 1.0f, 1.0f);
			bufferData.lightVar[0].direction = XMFLOAT3(-0.2f, -0.6f, -0.2f);
			bufferData.lightVar[0].color = XMFLOAT3(0.0f, 0.0f, 0.0f);
			bufferData.lightVar[0].attenuation = 1.0f;
			bufferData.lightVar[0].range = 1.0f;
			bufferData.lightVar[0].intensity = 1.0f;


			bufferData.lightVar[1].position = XMFLOAT3(0.f, 0.f, 0.f);
			bufferData.lightVar[1].direction = XMFLOAT3(0.f, 0.f, 0.f);
			bufferData.lightVar[1].color = XMFLOAT3(1.0f, 1.0f, 1.0f);
			bufferData.lightVar[1].attenuation = 1.0f;
			bufferData.lightVar[1].range = 500.0f;
			bufferData.lightVar[1].intensity = 1.0f;

			XMStoreFloat3(&bufferData.position, camera.r[3]);


#pragma region Sun

			myCon->PSSetConstantBuffers(0, 1, &cBuffList[1]);

			///////////////////////////////////////////////////////////////////////////////

			XMStoreFloat3(&bufferData.position, cameraVector);

			bufferData.timer = rot;
			//bufferData.timer = timer.TotalTime();

			hr = myCon->Map(cBuffList[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((cBuffData*)((&gpuBuffer)->pData)) = bufferData;
			myCon->Unmap(cBuffList[1], 0);
			///////////////////////////////////////////////////////////////////////////////

			//matrix math
			temporaryMatrix = XMMatrixIdentity();

			//temporaryMatrix = XMMatrixMultiply(XMMatrixTranslation(0, 60, 100), temporaryMatrix);
			temporaryMatrix = XMMatrixMultiply(XMMatrixScaling(40.f, 40.f, 40.f), temporaryMatrix);


			XMMATRIX sunPos = temporaryMatrix;

			XMStoreFloat4x4(&myMatrices.wMatrix, temporaryMatrix);

			myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			DisplayModel(
				&modelList[8],
				samplerState,
				cBuffList[0],
				myCon,
				&gpuBuffer,
				pSunShader,
				vMeshShader,
				layoutVert,
				myMatrices);

#pragma endregion
			bufferData.timer = 0.1f;
#pragma region Planet

			//matrix math
			temporaryMatrix = XMMatrixIdentity();
			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, sunPos);

			temporaryMatrix = XMMatrixMultiply(XMMatrixScaling(.01f, .01f, .01f), temporaryMatrix);
			temporaryMatrix = XMMatrixMultiply(XMMatrixTranslation(75, 0, 0), temporaryMatrix);

			temporaryMatrix = XMMatrixMultiply(XMMatrixRotationY(rot), temporaryMatrix);
			//temporaryMatrix = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationY(rot), sunPos), temporaryMatrix);

			//temporaryMatrix = XMMatrixMultiply(sunPos, temporaryMatrix);

			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixRotationY(rot / 6));
			

			XMMATRIX planetPos = temporaryMatrix;

			XMStoreFloat4x4(&myMatrices.wMatrix, temporaryMatrix);


			/////////////////////////////////////////////////////////////////////////

			myCon->PSSetConstantBuffers(0, 1, &cBuffList[1]);




			hr = myCon->Map(cBuffList[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((cBuffData*)((&gpuBuffer)->pData)) = bufferData;
			myCon->Unmap(cBuffList[1], 0);


			/////////////////////////////////////////////////////////////////////////


			myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			DisplayModel(
				&modelList[3],
				samplerState,
				cBuffList[0],
				myCon,
				&gpuBuffer,
				//pMeshShader,
				//vMeshShader,
				pLightShader,
				vLightShader,
				layoutVert,
				myMatrices);

#pragma endregion
#pragma region Moon

			//matrix math
			temporaryMatrix = XMMatrixIdentity();

			//temporaryMatrix = XMMatrixMultiply(XMMatrixTranslation(50, 0, 25), temporaryMatrix);


			temporaryMatrix = XMMatrixMultiply(XMMatrixTranslation(25, 0, 0), temporaryMatrix);
			

			temporaryMatrix = XMMatrixMultiply(XMMatrixScaling(.001f, .001f, .001f), temporaryMatrix);

			//temporaryMatrix = XMMatrixMultiply(XMMatrixRotationZ(3.14f / 3.f), temporaryMatrix);
			//temporaryMatrix = XMMatrixMultiply(XMMatrixRotationX(3.14f / 2.f), temporaryMatrix);

			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixRotationY(-rot / 4));
			temporaryMatrix = XMMatrixMultiply(temporaryMatrix, planetPos);

			//XMVECTOR moonVec = temporaryMatrix.r[3];

			//temporaryMatrix = XMMatrixMultiply(temporaryMatrix, XMMatrixRotationZ(rot));
			
			//temporaryMatrix.r[3] = moonVec;

			//moonVec = XMVector2Transform(moonVec, planetPos);
			//temporaryMatrix = XMMatrixMultiply(XMMatrixTranslationFromVector(planetPos), temporaryMatrix);

			XMStoreFloat4x4(&myMatrices.wMatrix, temporaryMatrix);

			/////////////////////////////////////////////////////////////////////////

			myCon->PSSetConstantBuffers(0, 1, &cBuffList[1]);


			hr = myCon->Map(cBuffList[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((cBuffData*)((&gpuBuffer)->pData)) = bufferData;
			myCon->Unmap(cBuffList[1], 0);


			/////////////////////////////////////////////////////////////////////////
			myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			DisplayModel(
				&modelList[7],
				samplerState,
				cBuffList[0],
				myCon,
				&gpuBuffer,
				//pMeshShader,
				//vMeshShader,
				pLightShader,
				vLightShader,
				layoutVert,
				myMatrices);

#pragma endregion
		}

		////////////////////////////////////////////////////
		mySwap->Present(1, 0);

		timer.Signal();
	}

	//Release all D3D11 interfaces

	for (unsigned int i = 0; i < cBuffList.size(); ++i)
	{
		cBuffList[i]->Release();
	}
	cBuffList.clear();

	for (unsigned int i = 0; i < skyboxTextures.size(); ++i)
	{
		skyboxTextures[i]->Release();
	}
	skyboxTextures.clear();

	skybox.iBufferData->Release();
	skybox.vBufferData->Release();

	myCon->Release();
	mySwap->Release();
	myDev->Release();
	myRtv->Release();

	pSkyboxShader->Release();
	pMeshShader->Release();
	pLightShader->Release();
	pColorShader->Release();
	pReflectionShader->Release();
	pSunShader->Release();

	vSkyboxShader->Release();
	vMeshShader->Release();
	vLightShader->Release();
	vColorShader->Release();
	vReflectionShader->Release();
	vTextureToGridShader->Release();

	gShader->Release();

	layoutVert->Release();

	samplerState->Release();
	zBuffer->Release();
	zBufferView->Release();

	for (unsigned int i = 0; i < modelList.size(); ++i)
	{
		if (modelList[i].texture)
			modelList[i].texture->Release();

		if (modelList[i].heightMap)
			modelList[i].heightMap->Release();

		if (modelList[i].iBufferData)
			modelList[i].iBufferData->Release();

		if (modelList[i].vBufferData)
			modelList[i].vBufferData->Release();
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHICS2PROJECT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GRAPHICS2PROJECT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
#pragma region initialization
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	RECT myWinR;
	GetClientRect(hWnd, &myWinR);

	// attach d3d11 to our window
	D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swap;
	ZeroMemory(&swap, sizeof(DXGI_SWAP_CHAIN_DESC));
	swap.BufferCount = 1;
	swap.OutputWindow = hWnd;
	swap.Windowed = true;
	swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap.BufferDesc.Width = myWinR.right - myWinR.left;
	swap.BufferDesc.Height = myWinR.bottom - myWinR.top;
	swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap.SampleDesc.Count = 1;

	aspectRatio = swap.BufferDesc.Width / float(swap.BufferDesc.Height);

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
		&dx11, 1, D3D11_SDK_VERSION, &swap, &mySwap, &myDev, 0, &myCon);

	ID3D11Resource* backbuffer;
	hr = mySwap->GetBuffer(0, _uuidof(backbuffer), (void**)&backbuffer);
	hr = myDev->CreateRenderTargetView(backbuffer, NULL, &myRtv);

	backbuffer->Release();

	myPort.Width = swap.BufferDesc.Width;
	myPort.Height = swap.BufferDesc.Height;
	myPort.TopLeftX = myPort.TopLeftY = 0;
	myPort.MinDepth = 0;
	myPort.MaxDepth = 1;

#pragma endregion

	MODEL_DATA tempModel;
	ID3D11Buffer* tempBuff = nullptr;
	OBJ_DATA ObjData;
	OBJ_VERTEX tempVertData;


	// load on cardf
	CD3D11_BUFFER_DESC bDesc;
	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	///////////////////////////////
	// CREATE CONSTANT BUFFER
	///////////////////////////////

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(WVP);
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;

	hr = myDev->CreateBuffer(&bDesc, nullptr, &tempBuff);

	cBuffList.push_back(tempBuff);

	//tempBuff->Release();

	////////////////////////////////////////////////////////

	//ZeroMemory(&bDesc, sizeof(bDesc));

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(cBuffData);
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;

	hr = myDev->CreateBuffer(&bDesc, nullptr, &tempBuff);

	cBuffList.push_back(tempBuff);

	///////////////////////////////
	// CREATE SAMPLER
	///////////////////////////////
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	hr = myDev->CreateSamplerState(&samplerDesc, &samplerState);

#pragma region CubeMap

	LoadOBJ(
		"assets/Misc Models/Skybox.obj",
		L"assets/Misc Models/SpaceCubemapTexture.dds",
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);
	skybox = tempModel;

	skyboxTextures.push_back(tempModel.texture);
	hr = CreateDDSTextureFromFile(myDev, L"assets/Misc Models/SkyTexture.dds", nullptr, &(tempModel.texture));
	skyboxTextures.push_back(tempModel.texture);
	hr = CreateDDSTextureFromFile(myDev, L"assets/Misc Models/FaceOfMarsTexture.dds", nullptr, &(tempModel.texture));
	skyboxTextures.push_back(tempModel.texture);
	hr = CreateDDSTextureFromFile(myDev, L"assets/Misc Models/FieldTexture.dds", nullptr, &(tempModel.texture));
	skyboxTextures.push_back(tempModel.texture);



#pragma endregion

#pragma region StoneHenge
	//////////////////////////////////////////////////////
	// HEADER MODEL LOADING : StoneHenge
	//////////////////////////////////////////////////////


	for (unsigned int i = 0; i < 1457; ++i)
	{
		tempVertData.pos.x = StoneHenge_data[i].pos[0];
		tempVertData.pos.y = StoneHenge_data[i].pos[1];
		tempVertData.pos.z = StoneHenge_data[i].pos[2];

		tempVertData.tex.x = StoneHenge_data[i].uvw[0];
		tempVertData.tex.y = StoneHenge_data[i].uvw[1];
		tempVertData.tex.z = StoneHenge_data[i].uvw[2];

		tempVertData.nrm.x = StoneHenge_data[i].nrm[0];
		tempVertData.nrm.y = StoneHenge_data[i].nrm[1];
		tempVertData.nrm.z = StoneHenge_data[i].nrm[2];

		ObjData.vertexList.push_back(tempVertData);
	}
	for (unsigned int i = 0; i < 2532; ++i)
	{
		ObjData.indexList.push_back(StoneHenge_indicies[i]);
	}


	LoadModelFromOBJ(
		L"assets/Stonehenge/StoneHenge.dds",
		ObjData,
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);

	modelList.push_back(tempModel);

	ObjData.indexList.clear();
	ObjData.vertexList.clear();


#pragma endregion 

#pragma region Pineapple

	LoadOBJ(
		"assets/Fruit/Pineapple/Pineapple.obj",
		L"assets/Fruit/Pineapple/PineSS00.dds",
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);
	modelList.push_back(tempModel);

#pragma endregion

#pragma region King

	LoadOBJ(
		"assets/Burger King/burgerking.obj",
		L"assets/Burger King/Tex_0134_0.dds",
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);
	modelList.push_back(tempModel);

#pragma endregion

#pragma region Planet

	LoadOBJ(
		"assets/Dinosaur Planet/Dinosaur Planet (World Map).obj",
		L"assets/Dinosaur Planet/MODELS.bin.001.000.dds",
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);
	modelList.push_back(tempModel);

#pragma endregion

#pragma region Procedural Model

	//----------------------------------------------------
	// Procedural Mesh : Grid
	//----------------------------------------------------




	MakeGrid(50, 1.0f, &ObjData, tempVertData);
	//MakeGrid(2, 1.0f, &ObjData, tempVertData);

	hr = CreateDDSTextureFromFile(myDev, L"assets/Misc Textures/Island.dds", nullptr, &(tempModel.heightMap));
	//hr = CreateDDSTextureFromFile(myDev, L"assets/Misc Textures/Island heightmap.dds", nullptr, &(tempModel.heightMap));

	LoadModelFromOBJ(
		L"assets/Misc Textures/Island Texture.dds",
		ObjData,
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);

	modelList.push_back(tempModel);
	ObjData.indexList.clear();
	ObjData.vertexList.clear();
	tempModel.heightMap = nullptr;

#pragma endregion

#pragma region Trash Can

	LoadOBJ(
		"assets/Trash Can/Trashcan.obj",
		L"assets/Trash Can/Trashcan.dds",
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);
	modelList.push_back(tempModel);

#pragma endregion

#pragma region Palm tree

	LoadOBJ(
		"assets/Palm tree/TREP.obj",
		L"assets/Palm tree/mt_tree_palm.dds",
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);
	modelList.push_back(tempModel);

#pragma endregion

#pragma region da moon

	LoadOBJ(
		"assets/Moon/moon.obj",
		L"assets/Moon/moon_00_0.dds",
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);
	modelList.push_back(tempModel);

#pragma endregion

#pragma region da sun

	LoadOBJ(
		"assets/Red Orb/Red Orb.obj",
		L"assets/Red Orb/MODELS.bin.036.000.dds",
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);
	modelList.push_back(tempModel);

#pragma endregion

#pragma region Water

	//----------------------------------------------------
	// Procedural Mesh : Grid
	//----------------------------------------------------

	MakeGrid(50, 1.0f, &ObjData, tempVertData);
	//MakeGrid(2, 1.0f, &ObjData, tempVertData);

	hr = CreateDDSTextureFromFile(myDev, L"assets/Misc Textures/Water Texture.dds", nullptr, &(tempModel.heightMap));

	LoadModelFromOBJ(
		L"assets/Misc Textures/Water Texture.dds",
		ObjData,
		&tempModel,
		&bDesc,
		&subData,
		myDev
	);

	modelList.push_back(tempModel);
	ObjData.indexList.clear();
	ObjData.vertexList.clear();
	tempModel.heightMap = nullptr;

#pragma endregion


	hr = myDev->CreatePixelShader(&MyMeshPShader, sizeof(MyMeshPShader), nullptr, &pMeshShader);
	hr = myDev->CreateVertexShader(&MyMeshVShader, sizeof(MyMeshVShader), nullptr, &vMeshShader);

	hr = myDev->CreatePixelShader(&MyPShader, sizeof(MyPShader), nullptr, &pLightShader);
	hr = myDev->CreateVertexShader(&MyVShader, sizeof(MyVShader), nullptr, &vLightShader);

	hr = myDev->CreatePixelShader(&ColorShiftPShader, sizeof(ColorShiftPShader), nullptr, &pColorShader);
	hr = myDev->CreateVertexShader(&ColorShiftVShader, sizeof(ColorShiftVShader), nullptr, &vColorShader);

	hr = myDev->CreatePixelShader(&SkyboxPShader, sizeof(SkyboxPShader), nullptr, &pSkyboxShader);
	hr = myDev->CreateVertexShader(&SkyboxVShader, sizeof(SkyboxVShader), nullptr, &vSkyboxShader);

	hr = myDev->CreatePixelShader(&ReflectionPShader, sizeof(ReflectionPShader), nullptr, &pReflectionShader);
	hr = myDev->CreateVertexShader(&ReflectionVShader, sizeof(ReflectionVShader), nullptr, &vReflectionShader);

	hr = myDev->CreateVertexShader(&TextureToGridVShader, sizeof(TextureToGridVShader), nullptr, &vTextureToGridShader);

	hr = myDev->CreatePixelShader(&SunPShader, sizeof(SunPShader), nullptr, &pSunShader);

	hr = myDev->CreateGeometryShader(&MyGShader, sizeof(MyGShader), nullptr, &gShader);

	//layout
	D3D11_INPUT_ELEMENT_DESC meshInputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = myDev->CreateInputLayout(meshInputDesc, 3, MyMeshVShader, sizeof(MyMeshVShader), &layoutVert);

	//create Z buffer & view
	D3D11_TEXTURE2D_DESC zDesc;
	ZeroMemory(&zDesc, sizeof(zDesc));
	zDesc.ArraySize = 1;
	zDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	zDesc.Width = swap.BufferDesc.Width;
	zDesc.Height = swap.BufferDesc.Height;
	zDesc.Format = DXGI_FORMAT_D32_FLOAT;
	zDesc.Usage = D3D11_USAGE_DEFAULT;
	zDesc.MipLevels = 1;
	zDesc.SampleDesc.Count = 1;
	hr = myDev->CreateTexture2D(&zDesc, nullptr, &zBuffer);
	hr = myDev->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);


	return TRUE;
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

bool MakeGrid(int halfSize, float spaceBetweenLines, OBJ_DATA* ObjData, OBJ_VERTEX tempVertData)
{

	// TODO : Fix edge not having line

	if (spaceBetweenLines <= 0)
		return false;
	if (halfSize <= 0)
		return false;

	int size = halfSize * 2;
	int multiplier = 1 / (spaceBetweenLines * spaceBetweenLines);
	int halfMultiplier = 1 / spaceBetweenLines;
	int totalVerts = (size * size * multiplier);
	int hIndices = totalVerts - (size * halfMultiplier);

	for (float i = -halfSize; i < halfSize; i += spaceBetweenLines) {
		for (float j = -halfSize; j < halfSize; j += spaceBetweenLines) {
			tempVertData.pos = { i, 0, j };
			tempVertData.tex = {
				((i + halfSize) / size),
				1 - ((j + halfSize) / size),
				0 };
			tempVertData.nrm = { 0, 1, 0 };

			ObjData->vertexList.push_back(tempVertData);
		}
	}

	for (unsigned int i = 1; i < totalVerts; ++i) {
		if (i <= hIndices && (i % (size * halfMultiplier)))
		{
			ObjData->indexList.push_back(i - 1);
			ObjData->indexList.push_back(i);
			ObjData->indexList.push_back(i + size);

			ObjData->indexList.push_back(i + size);
			ObjData->indexList.push_back(i + size - 1);
			ObjData->indexList.push_back(i - 1);
		}
	}
	/*	for (unsigned int i = 1; i < totalVerts; ++i) {
		if (!(i % (halfSize * 2 * halfMultiplier)))	{	}
		if ((i <= hIndices))	{
			ObjData->indexList.push_back(i - 1);
			ObjData->indexList.push_back(i + (halfSize * 2 * (halfMultiplier)-1)); }	}	*/
	return true;
}


/*bool MakeGrid(int halfSize, float spaceBetweenLines, OBJ_DATA* ObjData, OBJ_VERTEX tempVertData)
{

	if (spaceBetweenLines <= 0)
		return false;
	if (halfSize <= 0)
		return false;

	int multiplier = 1 / (spaceBetweenLines * spaceBetweenLines);
	int halfMultiplier = 1 / spaceBetweenLines;
	int totalVerts = (halfSize * halfSize * 4 * multiplier);
	int hIndices = totalVerts - (halfSize * 2 * halfMultiplier);

	for (float i = -halfSize; i < halfSize; i += spaceBetweenLines) {
		for (float j = -halfSize; j < halfSize; j += spaceBetweenLines) {
			tempVertData.pos = { i, 0, j };
			tempVertData.tex = { 0, 0, 0 };
			tempVertData.nrm = { 0, 1, 0 };

			ObjData->vertexList.push_back(tempVertData);
		}
	}

	for (unsigned int i = 1; i < totalVerts; ++i) {
		if ((i % (halfSize * 2 * halfMultiplier))) {
			ObjData->indexList.push_back(i - 1);
			ObjData->indexList.push_back(i);
		}

		if ((i <= hIndices))
		{
			ObjData->indexList.push_back(i - 1);
			ObjData->indexList.push_back(i + (halfSize * 2 * (halfMultiplier)-1));
		}
	}

	return true;
}*/