SamplerState SampleType;
Texture2D diffuse : register(t0);

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float3 tex  : OTEXCOORD;
};

// TODO : Make more interesting
float3 main(OutputVertex inputPixel) : SV_TARGET
{
	float4 textureColor;
	
	textureColor = diffuse.Sample(SampleType, inputPixel.tex);

	return textureColor;
}

