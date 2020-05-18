//#pragma pack_matrix(row_major)

struct InputVertex
{
    float4 xyzw : POSITION;
    float2 rgba : COLOR;
};

struct OutputVertex
{
    float4 xyzw : SV_POSITION;
    float2 rgba : OCOLOR;
};

cbuffer SHADER_VARS : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

OutputVertex main( InputVertex input )
{
    OutputVertex output = (OutputVertex)0;
    output.xyzw = input.xyzw;
    output.rgba = input.rgba;
    //math here

    output.xyzw = mul(worldMatrix, output.xyzw);
    output.xyzw = mul(viewMatrix,  output.xyzw);
    output.xyzw = mul(projMatrix,  output.xyzw);

	return output;
}


