

struct GSOutput
{
	float3 clr  : COLOR;
	float4 posH : SV_POSITION;
};

cbuffer SHADER_VARS : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

    [maxvertexcount(67)]

    void main(
        point GSOutput input[1], 
        inout TriangleStream<GSOutput> output)
{

        float halfBW = 0.1f;
        GSOutput simple[3];
        // colors
        simple[0].clr = float4(0.1, 0.6, 0.1, 1);
        simple[1].clr = float4(0, 1, 0, 1);
        simple[2].clr = float4(0.1, 0.6, 0.1, 1);
        // positions
        simple[0].posH = float4(input[0].posH.xyz, 1);
        simple[1].posH = float4(input[0].posH.xyz, 1);
        simple[2].posH = float4(input[0].posH.xyz, 1);
        // shift
        simple[0].posH.x -= halfBW;
        simple[1].posH.y += 0.6f;
        simple[2].posH.x += halfBW;
        
        for (uint i = 0; i < 3; ++i)
        {
            simple[i].posH = mul(simple[i].posH, projMatrix);
            output.Append(simple[i]);
        }
}