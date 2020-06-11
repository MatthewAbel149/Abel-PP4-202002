Texture2D diffuse : register(t0);
SamplerState sampleType : register(s0);

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float3 uvw  : OTEXCOORD;
};

cbuffer SHADER_VARS : register(b0)
{
	float3 position;
	float time;
};

// TODO : Make more interesting
float3 main(OutputVertex inputPixel) : SV_TARGET
{
	float4 textureColor;


	textureColor = diffuse.Sample(sampleType, inputPixel.uvw);
	
	textureColor *= float4(
	saturate( sin(time)),
	saturate( sin((position.y - 1.6) / 3)),
	saturate( cos((position.y - 1.6) / 3)),
	0
	);


	return textureColor;
}