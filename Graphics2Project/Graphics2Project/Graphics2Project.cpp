// Graphics2Project.cpp : Defines the entry point for the application.
//
#include "Includes.h"
//void LoadModel(CD3D11_BUFFER_DESC&, D3D11_SUBRESOURCE_DATA&, const unsigned int**, const OBJ_VERT**);

//#define SAFEDELETE(*ptr) if(ptr) { ptr. }

using namespace DirectX;
using namespace std;

struct MyVertex
{
	float pos[3];
	float uvw[3];
	float nrm[3];
};

unsigned int numVerts;

// Math Stuff
WVP myMatrices; //WVP struct was here

//ID3D11Buffer* vBuff;
//ID3D11InputLayout* vLayout;
//ID3D11PixelShader* pShader; //null
//ID3D11VertexShader* vShader; //null

// for init
ID3D11Device* myDev;
IDXGISwapChain* mySwap;
ID3D11DeviceContext* myCon;
// for drawing
ID3D11RenderTargetView* myRtv;
D3D11_VIEWPORT myPort;
float aspectRatio = 1;


ID3D11Buffer* cBuff; //shader variables
//complex mesh model variables
ID3D11Buffer* vBuffHeader;
ID3D11Buffer* iBuffHeader;
//header model variables
//ID3D11Buffer* vBuffHeader;
//ID3D11Buffer* iBuffHeader;

//complex mesh model data
ID3D11PixelShader* pMeshShader;
ID3D11VertexShader* vMeshShader;
ID3D11InputLayout* layoutVert;


vector< MODEL_DATA > modelList;

//needed for texturing
ID3D11ShaderResourceView* srvHeader;
ID3D11SamplerState* samplerState;
D3D11_SAMPLER_DESC samplerDesc;

// Z buffer
ID3D11Texture2D* zBuffer;
ID3D11DepthStencilView* zBufferView;

HRESULT hr;




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
#pragma region stuffIDontUnderstand
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

		// rendering here
		float color[] = { 0.0f, .0f, .8f, 1.f }; //color of clear buffer
		myCon->ClearRenderTargetView(myRtv, color); //clear buffer

		myCon->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);
		// Setup the pipeline

		// Output Merger
		ID3D11RenderTargetView* tempRTV[] = { myRtv }; //to remove, pass nullptr instead of myRtv
		myCon->OMSetRenderTargets(1, tempRTV, zBufferView);
		// Rasterizer
		myCon->RSSetViewports(1, &myPort);


#pragma region camera setup

		//world
		static float rot = 0;
		rot += 0.05f; //replace with timer
		XMMATRIX temp = XMMatrixIdentity();
		XMMATRIX temp2 = XMMatrixIdentity();
		temp = XMMatrixTranslation(-5, 10, -15);
		XMStoreFloat4x4(&myMatrices.wMatrix, temp);

#pragma region camera movement

		//view
		temp = XMMatrixLookAtLH({ 2,10,-20 }, { 0,0,3 }, { 0,1,0 });
		//temp = XMMatrixLookAtLH({ 0,10,-20 }, { 0,0,3 }, { 0,1,0 });
		XMStoreFloat4x4(&myMatrices.vMatrix, temp);

		temp = XMMatrixMultiply(temp2, temp);

#pragma endregion

		//projection
		temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, 0.1f, 1000);
		XMStoreFloat4x4(&myMatrices.pMatrix, temp);

#pragma endregion

		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		*((WVP*)(gpuBuffer.pData)) = myMatrices;
		//memcpy(gpuBuffer.pData, &myMatrices, sizeof(WVP));
		myCon->Unmap(cBuff, 0);

		ID3D11Buffer* constants[] = { cBuff };
		myCon->VSSetConstantBuffers(0, 1, constants);
		
		if (GetAsyncKeyState(VK_LBUTTON) & 1) scene += 1;
		if (scene == 3) scene -= 3;


	if (scene == 0) {
#pragma region StonehengeDrawCall
			//*Draw
			{
				//------------------- Load stonehenge
				UINT header_strides[] = { sizeof(_OBJ_VERT_) };
				UINT header_offsets[] = { 0 };
				ID3D11Buffer* headerVB[] = { vBuffHeader };

				myCon->IASetVertexBuffers(0, 1, headerVB, header_strides, header_offsets);
				myCon->IASetIndexBuffer(iBuffHeader, DXGI_FORMAT_R32_UINT, 0);
				myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


				//load the vertex shader for the mesh
				myCon->VSSetShader(vMeshShader, 0, 0);
				//load the pixel shader for the mesh
				myCon->PSSetShader(pMeshShader, 0, 0);
				//load the layout for the mesh
				myCon->IASetInputLayout(layoutVert);

				temp = XMMatrixIdentity();
				XMStoreFloat4x4(&myMatrices.wMatrix, temp);

				myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
				*((WVP*)(gpuBuffer.pData)) = myMatrices;
				myCon->Unmap(cBuff, 0);

				myCon->PSSetSamplers(0, 1, &samplerState);
				myCon->PSSetShaderResources(0, 1, &srvHeader);
				myCon->DrawIndexed(2532, 0, 0); //stonehenge
			}
			//*/
#pragma endregion

#pragma region King
			{
				//matrix math
				static float burgerRot = 0;

				if (GetAsyncKeyState('K'))
					burgerRot += .1f;

				temp = XMMatrixIdentity();

				temp2 = XMMatrixTranslation(0, 3, 8);
				temp = XMMatrixMultiply(temp2, temp);
				
				if (GetAsyncKeyState('B'))
				{
					temp2 = XMMatrixScaling(2.f, 2.f, 2.f);
					temp = XMMatrixMultiply(temp2, temp);
				}

				temp2 = XMMatrixRotationY(burgerRot);
				temp = XMMatrixMultiply(temp2, temp);

				XMStoreFloat4x4(&myMatrices.wMatrix, temp);

				DisplayModel(
					&modelList[1],
					samplerState,
					cBuff,
					myCon,
					&gpuBuffer,
					pMeshShader,
					vMeshShader,
					layoutVert,
					myMatrices);
			}
#pragma endregion
	}


	if (scene == 1) {
#pragma region Pineapple
			{
				//*------------------- Load complex mesh
				//matrix math
				temp = XMMatrixIdentity();

				temp2 = XMMatrixTranslation(-5, 10, 0);
				temp = XMMatrixMultiply(temp2, temp);

				temp2 = XMMatrixScaling(.1f, .1f, .1f);
				temp = XMMatrixMultiply(temp2, temp);

				temp2 = XMMatrixRotationY(rot);
				temp = XMMatrixMultiply(temp2, temp);

				XMStoreFloat4x4(&myMatrices.wMatrix, temp);


				DisplayModel(
					&modelList[0],
					samplerState,
					cBuff,
					myCon,
					&gpuBuffer,
					pMeshShader,
					vMeshShader,
					layoutVert,
					myMatrices);
			}
#pragma endregion
	}


	if (scene == 2) {
#pragma region Planet

			//matrix math
			temp = XMMatrixIdentity();

			temp2 = XMMatrixTranslation(0, 16, 14);
			temp = XMMatrixMultiply(temp2, temp);

			temp2 = XMMatrixRotationY(rot / 10);
			temp = XMMatrixMultiply(temp2, temp);

			XMStoreFloat4x4(&myMatrices.wMatrix, temp);


			DisplayModel(
				&modelList[2],
				samplerState,
				cBuff,
				myCon,
				&gpuBuffer,
				pMeshShader,
				vMeshShader,
				layoutVert,
				myMatrices);

#pragma endregion
	}

		mySwap->Present(1, 0);
	}

	//Release all D3D11 interfaces

	cBuff->Release();
	myCon->Release();
	mySwap->Release();
	myDev->Release();
	myRtv->Release();
	///
	//vBuffList->Release();
	//iBuffList->Release();
	//pShaderList->Release();
	//vShaderList->Release();
	//layoutList->Release();
	//srvList->Release();

	vBuffHeader->Release();
	iBuffHeader->Release();
	
	pMeshShader->Release();
	vMeshShader->Release();
	layoutVert->Release();
	
	srvHeader->Release();

	samplerState->Release();
	zBuffer->Release();
	zBufferView->Release();

	for (unsigned int i = 0; i < modelList.size(); ++i) 
	{ 
		modelList[i].iBufferData->Release();
		modelList[i].vBufferData->Release();
		modelList[i].srvData->Release();
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

	const wchar_t* texturePath = { L"assets/Dinosaur Planet/MODELS.bin.001.000.dds" };
	texturePath = { L"assets/Fruit/Pineapple/PineSS00.dds" };
	//texturePath = { L"assets/Dinosaur Planet/MODELS.bin.001.000.dds" };




	// load on cardf
	CD3D11_BUFFER_DESC bDesc;
	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	//--------------------
	// create constant buffer

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(WVP);
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;

	hr = myDev->CreateBuffer(&bDesc, nullptr, &cBuff);

	//--------------------
	// Create sampler
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	hr = myDev->CreateSamplerState(&samplerDesc, &samplerState);

#pragma region StoneHenge
	//*----------------------------------------------------
	//header model loading : StoneHenge
		//buffer desc
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(StoneHenge_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	subData.pSysMem = StoneHenge_data; //from header

	
	hr = myDev->CreateBuffer(&bDesc, &subData, &vBuffHeader);

	//index buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(StoneHenge_indicies);
	subData.pSysMem = StoneHenge_indicies;

	
	hr = myDev->CreateBuffer(&bDesc, &subData, &iBuffHeader);

	hr = CreateDDSTextureFromFile(myDev, L"assets/Stonehenge/StoneHenge.dds", nullptr, &srvHeader);
	//----------------------------------------------------*/
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


	hr = myDev->CreatePixelShader(&MyMeshPShader, sizeof(MyMeshPShader), nullptr, &pMeshShader);
	hr = myDev->CreateVertexShader(&MyMeshVShader, sizeof(MyMeshVShader), nullptr, &vMeshShader);

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
