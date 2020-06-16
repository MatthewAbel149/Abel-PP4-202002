textureCUBE diffuse : register(t0);

SamplerState sampleType : register(s0);

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float3 uvw  : OTEXCOORD;
};

// TODO : Make more interesting
float3 main(OutputVertex inputPixel) : SV_TARGET
{
	float4 textureColor = diffuse.Sample(sampleType, inputPixel.uvw);

	return textureColor;
}