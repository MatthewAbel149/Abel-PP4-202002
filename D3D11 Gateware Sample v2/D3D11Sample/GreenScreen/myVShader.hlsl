struct InputVertex
{
	float4 xyzw : POSITION;
	float4 rgbx : COLOR;
};

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float4 rgbx : COLOR;
};

OutputVertex main(InputVertex input)
{
	OutputVertex output = (OutputVertex)0;
	output.xyzw = input.xyzw;
	//math


	return output;
}