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


	//textureColor = diffuse.Sample(sampleType, inputPixel.uvw);
	//textureColor.xyzw = float4(3, 3, 112, 1);
	
	//textureColor = float4(cos(time), 0, 0, 0); //sin(time), tan(time), 0);
textureColor = float4(
	sin(position.x),
	sin(position.y),
	sin(time),
	0);


	return textureColor;
}