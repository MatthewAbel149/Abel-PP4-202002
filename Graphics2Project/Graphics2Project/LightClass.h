#pragma once
#include <DirectXMath.h>


class Light
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 color;

	float intensity = 0.5f;
	float range = 5.0f;
	float attenuation = 0.1f;
	
	void SetPosition(float x, float y, float z);
	void SetDirection(float x, float y, float z);
	void SetColor(float r, float g, float b);
};



/*
class PointLight
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;

	float intensity = 0;
	float range = 0;
	float attenuation = 0;


	//LIGHTDIR = NORMALIZE( LIGHTPOS – SURFACEPOS )
	//LIGHTRATIO = CLAMP( DOT( LIGHTDIR, SURFACENORMAL ) )
	//RESULT = LIGHTRATIO * LIGHTCOLOR * SURFACECOLOR

};

class DirectionalLight
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 color;

	float intensity = 0;

	//LIGHTRATIO = CLAMP( DOT( -LIGHTDIR, SURFACENORMAL ) )
	//RESULT = LIGHTRATIO * LIGHTCOLOR * SURFACECOLOR
};

class SpotLight
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 color;
	float phi = 0;
	float theta = 0;

	float intensity = 0;

	//LIGHTDIR = NORMALIZE( LIGHTPOS – SURFACEPOS ) )
	//SURFACERATIO = CLAMP( DOT( -LIGHTDIR, CONEDIR ) )
	//SPOTFACTOR = ( SURFACERATIO > CONERATIO ) ? 1 : 0
	//LIGHTRATIO = CLAMP( DOT( LIGHTDIR, SURFACENORMAL ) )
	//RESULT = SPOTFACTOR * LIGHTRATIO * LIGHTCOLOR * SURFACECOLOR

};

*/