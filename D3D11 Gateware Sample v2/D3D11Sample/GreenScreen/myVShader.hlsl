struct myVertex
{
	xyzw[4];
	rgbx[4];
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}