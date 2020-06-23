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
	float3 textureCoord;

	textureCoord.x = sin(timer) * 0.05f + inputPixel.uvw.x;
	textureCoord.y = cos(timer) * 0.05f + inputPixel.uvw.y;
	//textureCoord.x = sin(timer);
	//textureCoord.y = cos(timer);
	textureCoord.z = 0;

	textureColor = diffuse.Sample(sampleType, textureCoord);

	return textureColor;
}