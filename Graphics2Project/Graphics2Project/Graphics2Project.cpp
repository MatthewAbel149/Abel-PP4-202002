// Graphics2Project.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Graphics2Project.h"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "MyPShader.csh"
#include "MyVShader.csh"



// for init
ID3D11Device* myDev;
IDXGISwapChain* mySwap;
ID3D11DeviceContext* myCon;

// for drawing
ID3D11RenderTargetView* myRtv;
D3D11_VIEWPORT myPort;

struct MyVertex
{
    float xyzw[4];
    float rgba[4];
};

ID3D11Buffer* vBuff;
ID3D11InputLayout* vLayout; 
ID3D11PixelShader* pShader; //null
ID3D11VertexShader* vShader; //null

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
        float color[] = {0.f, 0.f, 0.6f, 1.f};
        myCon->ClearRenderTargetView(myRtv, color);

        // Setup the pipeline

        // Output Merger
        ID3D11RenderTargetView* tempRTV[] = { myRtv }; //to remove, pass nullptr instead of myRtv
        myCon->OMSetRenderTargets(1, tempRTV, nullptr);
        // Rasterizer
        myCon->RSSetViewports(1, &myPort);
        // Input Addembler
        myCon->IASetInputLayout(vLayout);

        UINT strides[] = {sizeof(MyVertex)};
        UINT offsets[] = { 0 };
        ID3D11Buffer* tempVB[] = { vBuff };
        myCon->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
        myCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //vertex shader stage
        myCon->VSSetShader(vShader, 0, 0);
        //pixel shader stage
        myCon->PSSetShader(pShader, 0, 0);

        //Draw
        myCon->Draw(3, 0);

        mySwap->Present(1, 0);
    }

    //Release all our D3D11 interfaces
    myCon->Release();
    mySwap->Release();
    myDev->Release();
    myRtv->Release();
    vBuff->Release();
    //pShader->Release();
    //vShader->Release();
    

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
   //swap.SampleDesc

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
    
    MyVertex poly[3] = //NDC
    {
        { {0, 0.5f, 0, 1}, {1,1,1,1} },
        { {0.5f, -0.5f, 0, 1}, {1,1,1,1} },
        { {-0.5f, -0.5f, 0, 1}, {1,1,1,1} }
    };

    // load on cardf
    CD3D11_BUFFER_DESC bDesc;
    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&bDesc, sizeof(bDesc));
    ZeroMemory(&subData, sizeof(subData));

    bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bDesc.ByteWidth = sizeof(poly) * 3;
    bDesc.CPUAccessFlags = 0;
    bDesc.StructureByteStride = 0;
    bDesc.Usage = D3D11_USAGE_DEFAULT;

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
