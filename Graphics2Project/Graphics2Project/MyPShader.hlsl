Texture2D diffuse : register(t0);
SamplerState sampleType : register(s0);

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float3 uvw  : OTEXCOORD;
	float3 nrm  : NORMAL0;

	float3 WrlPos : POSITION;
};

cbuffer SHADER_VARS : register(b0)
{
	struct {

		float3 position;
		float intensity;
		float3 direction;
		float range;
		float3 color;
		float attenuation;

	} light;
	float3 cameraPos;
	float timer;

};

// TODO : Make more interesting
float3 main(OutputVertex inputPixel) : SV_TARGET
{
	float4 textureColor;
	textureColor = diffuse.Sample(sampleType, inputPixel.uvw);




	////////////////////////////////////////////////////
	//	Directional Light
	////////////////////////////////////////////////////

	float3 normalizedLightDir = normalize(light.direction);

	inputPixel.nrm = normalize(inputPixel.nrm);

	float lightratio = saturate(dot(-normalizedLightDir, inputPixel.nrm));
	float3 result0 = float3(lightratio * light.color * textureColor.xyz); //use light color


	////////////////////////////////////////////////////
	//	End of Directional Light
	////////////////////////////////////////////////////

	////////////////////////////////////////////////////
	//	Point Light
	////////////////////////////////////////////////////

	//
	////
	//Light light1;

	//light1.position  = float3( 5.0f,  2.0f, -5.0f );
	//light1.color	   = float3( 1.0f,  1.0f,  0.0f );
	//light1.range	   = float ( 15.0f );
	//

	//float3 lightDir  = (light1.position.xyz - inputPixel.WrlPos);
	//float distance   = length(lightDir);
	//lightDir		   = lightDir / distance; //normalized direction vector
	//float limit	   = 1 - saturate(distance / light1.range);

	//limit = limit * limit; // exponential


	//lightratio			= saturate(dot(lightDir, inputPixel.nrm));
	//float3 result1		= limit * lightratio * light1.color * textureColor;

	////////////////////////////////////////////////////
	//	End of Point Light
	////////////////////////////////////////////////////

	//
	//textureColor.xyz = result0 + result1;


	textureColor.xyz = result0;
	//textureColor.xyz = light.color;
	return textureColor;
}