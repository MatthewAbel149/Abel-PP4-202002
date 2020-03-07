struct InputVertex
{
    float4 xyzw : POSITION;
    float4 rgba : COLOR;
};

struct OutputVertex
{
    float4 xyzw : SV_POSITION;
};

OutputVertex main( InputVertex input )
{
    OutputVertex output = (OutputVertex)0;
    output.xyzw = input.xyzw;
    //math here

	return output;
}