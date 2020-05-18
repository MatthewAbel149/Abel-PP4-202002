// Graphics2Project.cpp : Defines the entry point for the application.
//
#include "Includes.h"

// for init
ID3D11Device* myDev;
IDXGISwapChain* mySwap;
ID3D11DeviceContext* myCon;

// for drawing
ID3D11RenderTargetView* myRtv;
D3D11_VIEWPORT myPort;
float aspectRatio = 1;

struct MyVertex
{
    float xyzw[4];
    float rgba[4];
};

unsigned int numVerts;

ID3D11Buffer* vBuff;
ID3D11InputLayout* vLayout; 
ID3D11PixelShader* pShader; //null
ID3D11VertexShader* vShader; //null

ID3D11Buffer* cBuff; //shader variables

ID3D11Buffer* vBuffMesh;
ID3D11Buffer* iBuffMesh;

ID3D11PixelShader* pMeshShader;
ID3D11VertexShader* vMeshShader;
ID3D11InputLayout* vMeshLayout; 

//ID3D11Resource* textureResource; //needed for texturing
ID3D11ShaderResourceView* srv; //needed for texturing


// Z buffer
ID3D11Texture2D* zBuffer;
ID3D11DepthStencilView* zBufferView;

// Math Stuff
struct WVP {
XMFLOAT4X4 wMatrix;
XMFLOAT4X4 vMatrix;
XMFLOAT4X4 pMatrix;
} myMatrices;


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
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPHICS2PROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICS2PROJECT));

    MSG msg;

    ////////////////////////////////////////////

    HRESULT hr;

    //textureView = 0;
    //textureResource = 0;

    //if (!textureResource && !srv)
    //hr = CreateDDSTextureFromFile(myDev, myCon, to_wstring(pFilename).c_str(), nullptr, &srv);
    //hr = DirectX::CreateDDSTextureFromFile(m_Device, m_Context, to_wstring(pFilename).c_str(), nullptr, &srv);
	hr = CreateDDSTextureFromFile(myDev, L"assets/Fruit/Pineapple/PineSS00.dds", nullptr, &srv);
    //srv->GetDesc();

    ////////////////////////////////////////////

    // Main message loop:
    while (true) //GetMessage(&msg, nullptr, 0, 0))
    {
        PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT)
            break;

        // rendering here
        float color[] = { 0.f, 0.f, 0.6f, 1.f };
        myCon->ClearRenderTargetView(myRtv, color);

        myCon->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);
        // Setup the pipeline

        // Output Merger
        ID3D11RenderTargetView* tempRTV[] = { myRtv }; //to remove, pass nullptr instead of myRtv
        myCon->OMSetRenderTargets(1, tempRTV, zBufferView);
        // Rasterizer
        myCon->RSSetViewports(1, &myPort);
        // Input Addembler
        myCon->IASetInputLayout(vLayout);

        UINT strides[] = { sizeof(MyVertex) };
        UINT offsets[] = { 0 };
        ID3D11Buffer* tempVB[] = { vBuff };
        myCon->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
        myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //vertex shader stage
        myCon->VSSetShader(vShader, 0, 0);
        //pixel shader stage
        myCon->PSSetShader(pShader, 0, 0);


        
        //world
        static float rot = 0; rot += 0.05f; //replace with timer
        XMMATRIX temp = XMMatrixIdentity();
        temp = XMMatrixTranslation(-5, 10, -15);
        XMMATRIX temp2 = XMMatrixRotationY(rot);
        temp = XMMatrixMultiply(temp2, temp);
        XMStoreFloat4x4(&myMatrices.wMatrix, temp);
        
        //view
        temp = XMMatrixLookAtLH({ 2,10,-20 }, { 0,0,3 }, { 0,1,0 });
        XMStoreFloat4x4(&myMatrices.vMatrix, temp);
        
        //projection
        temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, 0.1f, 1000);
        XMStoreFloat4x4(&myMatrices.pMatrix, temp);



        D3D11_MAPPED_SUBRESOURCE gpuBuffer;
        myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        *((WVP*)(gpuBuffer.pData)) = myMatrices;
        //memcpy(gpuBuffer.pData, &myMatrices, sizeof(WVP));
        myCon->Unmap(cBuff, 0);

        ID3D11Buffer* constants[] = { cBuff };
        myCon->VSSetConstantBuffers(0, 1, constants);

        //Draw
        myCon->Draw(numVerts, 0);


        UINT mesh_strides[] = { sizeof(_OBJ_VERT_) };
        UINT mesh_offsets[] = { 0 };
        ID3D11Buffer* meshVB[] = { vBuffMesh };

        myCon->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
        myCon->IASetIndexBuffer(iBuffMesh, DXGI_FORMAT_R32_UINT, 0);
        myCon->VSSetShader(vMeshShader, 0, 0);
        myCon->PSSetShader(pMeshShader, 0, 0);
        myCon->IASetInputLayout(vMeshLayout);

        temp = XMMatrixIdentity();
        temp = XMMatrixScaling(.1f, .1f, .1f);
        temp = XMMatrixMultiply(temp2, temp);
        //temp = XMMatrixTranslation(-5, 10, -15);

        XMStoreFloat4x4(&myMatrices.wMatrix, temp);

        //view
        //temp = XMMatrixLookAtLH({ 2,10,-20 }, { 0,0,3 }, { 0,1,0 });
        //XMStoreFloat4x4(&myMatrices.vMatrix, temp);

        //projection
        //temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectRatio, 0.1f, 1000);
        //XMStoreFloat4x4(&myMatrices.pMatrix, temp);
        
        myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        *((WVP*)(gpuBuffer.pData)) = myMatrices;
        myCon->Unmap(cBuff, 0);

		myCon->PSSetShaderResources(0, 1, &srv);
        //myCon->DrawIndexed(2532, 0, 0); //stonehenge
        myCon->DrawIndexed(2742, 0, 0); //pineapple

        mySwap->Present(1, 0);
    }

    //Release all our D3D11 interfaces
    myCon->Release();
    mySwap->Release();
    myDev->Release();
    myRtv->Release();
    vBuff->Release();
    pShader->Release();
    vShader->Release();
    vLayout->Release();
    vMeshLayout->Release();
    cBuff->Release();
    vBuffMesh->Release();
    iBuffMesh->Release();
    pMeshShader->Release();
    vMeshShader->Release();
    //textureResource->Release();
    srv->Release();
    zBuffer->Release();
    zBufferView->Release();
    

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHICS2PROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GRAPHICS2PROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   HRESULT hr;
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
    
    MyVertex poly[] = //NDC
    {
        //front
        { { 0.0f,    1.0f,   0.0f, 1}, {1,1,1,1} },
        { { 0.25f, -0.25f, -0.25f, 1}, {1,0,1,1} },
        { {-0.25f, -0.25f, -0.25f, 1}, {1,1,0,1} },
        //Right side
        { { 0.0f,    1.0f,   0.0f, 1}, {1,0,1,1} },
        { { 0.25f, -0.25f,  0.25f, 1}, {1,1,0,1} },
        { { 0.25f, -0.25f, -0.25f, 1}, {1,1,1,1} },
        //back
        { {   0.0f,   1.0f,  0.0f, 1}, {1,1,1,1} },
        { { -0.25f, -0.25f, 0.25f, 1}, {1,0,1,1} },
        { {  0.25f, -0.25f, 0.25f, 1}, {1,1,0,1} },
        //left side   
        { {   0.0f,   1.0f,   0.0f, 1}, {1,0,1,1} },
        { { -0.25f, -0.25f, -0.25f, 1}, {1,1,0,1} },
        { { -0.25f, -0.25f,  0.25f, 1}, {1,1,1,1} },
    };
    numVerts = ARRAYSIZE(poly);

    // load on cardf
    CD3D11_BUFFER_DESC bDesc;
    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&bDesc, sizeof(bDesc));
    ZeroMemory(&subData, sizeof(subData));

    bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bDesc.ByteWidth = sizeof(MyVertex) * numVerts;
    bDesc.CPUAccessFlags = 0;
    bDesc.MiscFlags = 0;
    bDesc.StructureByteStride = 0;
    bDesc.Usage = D3D11_USAGE_IMMUTABLE;

    subData.pSysMem = poly;

    hr = myDev->CreateBuffer(&bDesc, &subData, &vBuff);

    //write and compile and load shaders
    hr = myDev->CreateVertexShader(MyVShader, sizeof(MyVShader), nullptr, &vShader);
    hr = myDev->CreatePixelShader(MyPShader, sizeof(MyPShader), nullptr, &pShader);

    //describe vertex D3D11
    D3D11_INPUT_ELEMENT_DESC ieDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    hr = myDev->CreateInputLayout(ieDesc, 2, MyVShader, sizeof(MyVShader), &vLayout);

    // create constant buffer
    ZeroMemory(&bDesc, sizeof(bDesc));
    
    bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bDesc.ByteWidth = sizeof(WVP);
    bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bDesc.MiscFlags = 0;
    bDesc.StructureByteStride = 0;
    bDesc.Usage = D3D11_USAGE_DYNAMIC;

    hr = myDev->CreateBuffer(&bDesc, nullptr, &cBuff);

	///////////////////////////////////////////////////////
    //complex mesh
    bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bDesc.ByteWidth = sizeof(Pineapple_data);
    bDesc.CPUAccessFlags = 0;
    bDesc.MiscFlags = 0;
    bDesc.StructureByteStride = 0;
    bDesc.Usage = D3D11_USAGE_IMMUTABLE;

    subData.pSysMem = Pineapple_data;

    hr = myDev->CreateBuffer(&bDesc, &subData, &vBuffMesh);

    //index buffer mesh
    bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bDesc.ByteWidth = sizeof(Pineapple_indicies);
    subData.pSysMem = Pineapple_indicies;
    hr = myDev->CreateBuffer(&bDesc, &subData, &iBuffMesh); 

    //load the vertex shader for the mesh
	//myDev->CreateShaderResourceView(nullptr, nullptr, &srv);
	//myDev->CreateTexture2D();
    hr = myDev->CreateVertexShader(MyMeshVShader, sizeof(MyMeshVShader), nullptr, &vMeshShader);
    hr = myDev->CreatePixelShader(MyMeshPShader, sizeof(MyMeshPShader), nullptr, &pMeshShader);

    D3D11_INPUT_ELEMENT_DESC meshInputDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    hr = myDev->CreateInputLayout(meshInputDesc, 3, MyMeshVShader, sizeof(MyMeshVShader), &vMeshLayout);

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

/*OBJLoad() {
    vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    vector< glm::vec3 > temp_vertices;
    vector< glm::vec2 > temp_uvs;
    vector< glm::vec3 > temp_normals;

};
*/

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
