Texture2D diffuse : register(t0);
SamplerState sampleType : register(s0);

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float3 uvw  : OTEXCOORD;
	float3 nrm  : NORMAL0;
};
struct LightStruct {

	float3 position;
	float intensity;
	float3 direction;
	float range;
	float3 color;
	float attenuation;

};
cbuffer SHADER_VARS : register(b0)
{
	LightStruct light[2];
	float3 cameraPos;
	float timer;
};

// TODO : Make more interesting
float3 main(OutputVertex inputPixel) : SV_TARGET
{
	float4 textureColor;
	textureColor = diffuse.Sample(sampleType, inputPixel.uvw);
	
	if (timer == 1)
	{
		textureColor.w = 0;
	}

	if (textureColor.w <= .1)
	{
		discard;
	}


	//textureColor.x = textureColor.x * 0.3f;
	//textureColor.y = textureColor.y * 0.3f;
	//textureColor.z = textureColor.z * 0.3f;

	return textureColor;
}