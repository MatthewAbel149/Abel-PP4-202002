Texture2D diffuse : register(t0);
textureCUBE cubemap : register(t1);
SamplerState sampleType : register(s0);

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float3 uvw  : OTEXCOORD;
	float3 nrm	: NORMAL0;

	float3 WrlPos : POSITION;
};

cbuffer SHADER_VARS : register(b0)
{
	struct {
		float3 position;
		float intensity;
		float3 direction;
		float range;
		float3 color;
		float attenuation;

	} light;
	float3 cameraPos;
	float timer;
};

float3 main(OutputVertex inputPixel) : SV_TARGET
{
	float4 textureColor = diffuse.Sample(sampleType, inputPixel.uvw);


	float3 texCoord = (inputPixel.WrlPos - cameraPos);

	texCoord = normalize(texCoord);
	inputPixel.nrm = normalize(inputPixel.nrm);

	//float3 vectorRet = reflect(inputPixel.nrm, texCoord);
	float3 vectorRet = reflect(texCoord, inputPixel.nrm);

	vectorRet = normalize(vectorRet);
	float4 reflectedColor = cubemap.Sample(sampleType, vectorRet);

	textureColor = saturate(textureColor + (timer * reflectedColor));


	return textureColor;
}