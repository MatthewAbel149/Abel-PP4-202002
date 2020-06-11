#pragma once
#include "framework.h"
#include "Graphics2Project.h"


//#include <d3d11.h>
//#pragma comment(lib, "d3d11.lib")
//#include <DirectXMath.h>
#include "OBJLoader.h"
using namespace DirectX;
using namespace std;
#include <iostream>
#include <DirectXMathMatrix.inl>
#include "XTime.h"
#include <zmouse.h>

//header model shaders
#include "HeaderPShader.csh"
#include "HeaderVShader.csh"
//complex mesh shaders
#include "MyMeshPShader.csh"
#include "MyMeshVShader.csh"
//lighting shaders
#include "MyPShader.csh"
#include "MyVShader.csh"
 
#include "assets/Stonehenge/StoneHenge.h"