Texture2D diffuse : register(t0);
SamplerState sampleType : register(s0);

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
    float3 nrm  : NORMAL0;

    float3 WrlPos : POSITION;
};

cbuffer SHADER_VARS : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

OutputVertex main(InputVertex input)
{
    float height = (diffuse.SampleLevel(sampleType, input.uvw, 0)).x * 10;

    OutputVertex output = (OutputVertex)0;
    output.xyzw = float4(input.pos.x, input.pos.y + height, input.pos.z, 1);


    output.xyzw = mul(worldMatrix, output.xyzw);
    output.WrlPos = output.xyzw.xyz;
    
    output.xyzw = mul(viewMatrix, output.xyzw);
    output.xyzw = mul(projMatrix, output.xyzw);


    output.uvw.xyz = input.uvw;
    output.nrm.xyz = input.nrm.xyz;

    return output;
}