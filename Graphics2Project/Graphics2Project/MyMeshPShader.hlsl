Texture2D diffuse : register(t0);
SamplerState sampleType : register(s0);

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float3 uvw  : OTEXCOORD;
};

// TODO : Make more interesting
float3 main(OutputVertex inputPixel) : SV_TARGET
{
	float4 textureColor;


	textureColor = diffuse.Sample(sampleType, inputPixel.uvw);
	//textureColor.xyzw = float4(3, 3, 112, 1);

	return textureColor;
}

