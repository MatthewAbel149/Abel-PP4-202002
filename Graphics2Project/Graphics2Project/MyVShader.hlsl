struct InputVertex
{
    float xyzw[4] : POSITION;
    float rgba[4] : COLOR;
};

struct OutputVertex
{
    float xyzw[4] : SV_POSITION;
};

OutputVertex main( InputVertex input )
{
    OutputVertex output = (OutputVertex)0;
    output.xyzw = input.xyzw;
    //math here

	return output;
}