struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float4 rgba : OCOLOR;
};

// TODO : Make more interesting
float4 main(OutputVertex inputPixel) : SV_TARGET
{
	//return inputPixel.rgba;
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}