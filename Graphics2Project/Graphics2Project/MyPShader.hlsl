Texture2D diffuse : register(t0);
SamplerState sampleType : register(s0);

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float3 uvw  : OTEXCOORD;
	float3 nrm  : NORMAL0;

	float3 WrlPos : POSITION;
};


struct Light
{
	float3 position;
	float3 direction;
	float3 color;

	float intensity;
	float range;
	float attenuation;
};


// TODO : Make more interesting
float3 main(OutputVertex inputPixel) : SV_TARGET
{
	float4 textureColor;


	textureColor = diffuse.Sample(sampleType, inputPixel.uvw);


	Light light0;
	light0.direction = float3(0.0f, -0.9f, -0.03f);
	light0.color = float3(0.0f, 0.0f, 1.0f);
	//light0.color = float3(0.0f, 0.0f, 1.0f);

	float lightratio = saturate(dot(-light0.direction, inputPixel.nrm));
	float3 result0 = lightratio * (light0.color.xyz * textureColor.xyz);

	
	//
	Light light1;

	light1.position	   = float3(3.0f,  3.0f, -3.0f);
	light1.color	   = float3(1.0f,  1.0f,  0.0f);
	light1.range	   = float (60.0f);
	
	//light1.intensity   = float (0.7f);
	//light1.attenuation = float (0.1f);


	float3 lightDir     = (light1.position.xyz - inputPixel.WrlPos);
	float distance		= length(lightDir);
	lightDir			= lightDir / distance; //normalized direction vector
	float limit			= 1 - saturate(distance / light1.range);

	limit = limit * limit; // exponential


	lightratio			= saturate(dot(lightDir, inputPixel.nrm));
	float3 result1		= limit * lightratio * light1.color * textureColor;
	
	textureColor.xyz = result0 + result1;

	//textureColor = float4(.4, 0, .4, 1);
	return textureColor;
}