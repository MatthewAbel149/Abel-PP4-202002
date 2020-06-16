//#pragma pack_matrix(row_major)

struct InputVertex
{
    float3 pos : POSITION;
    float3 uvw : TEXCOORD;
    float3 nrm : NORMAL;
};

struct OutputVertex
{
    float4 xyzw : SV_POSITION;
    float3 uvw  : OTEXCOORD;
};

cbuffer SHADER_VARS : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

OutputVertex main(InputVertex input)
{
    OutputVertex output = (OutputVertex)0;
    output.xyzw = float4(input.pos, 1);
    output.uvw = input.pos;

    output.xyzw = mul(worldMatrix, output.xyzw);
    output.xyzw = mul(viewMatrix, output.xyzw);
    output.xyzw = mul(projMatrix, output.xyzw);

    return output;
}