Texture2D diffuse : register(t0);
SamplerState sampleType : register(s0);

struct OutputVertex
{
	float4 xyzw : SV_POSITION;
	float3 uvw  : OTEXCOORD;
	float3 nrm  : NORMAL0;

	float3 WrlPos : POSITION;
};

struct LightStruct {

	float3 position;
	float intensity;
	float3 direction;
	float range;
	float3 color;
	float attenuation;

};
cbuffer SHADER_VARS : register(b0)
{
	LightStruct light[2];
	float3 cameraPos;
	float timer;

};

// TODO : Make more interesting
float3 main(OutputVertex inputPixel) : SV_TARGET
{
	float4 textureColor;
	textureColor = diffuse.Sample(sampleType, inputPixel.uvw);

	inputPixel.nrm = normalize(inputPixel.nrm);



	float3 ambientColor;
	if (timer <= 1);
		ambientColor = textureColor * timer;

	float3 result[2];
	for (unsigned int i = 0; i < 2; i++)
	{
		result[i] = float3(0, 0, 0);
	}


	/////////////////////////////////
	//need to add dynamically:
	float specularIntensity = 0.8f;
	float specularPower		= 50.f;

	/////////////////////////////////


	for (unsigned int i = 0; i < 2; i++)
	{

		if (!(light[i].color.x == 0) ||
			!(light[i].color.y == 0) ||
			!(light[i].color.z == 0)) {

			////////////////////////////////////////////////////
			//	Directional Light
			////////////////////////////////////////////////////
			if (!(light[i].direction.x == 0) &&
				!(light[i].direction.y == 0) &&
				!(light[i].direction.z == 0))
		{
			float3 normalizedLightDir = normalize(light[i].direction);

			float lightratio = saturate(dot(-normalizedLightDir, inputPixel.nrm));
			result[i] = float3(lightratio * light[i].color * textureColor.xyz); //use light color
		}

			////////////////////////////////////////////////////
			//	End of Directional Light
			////////////////////////////////////////////////////

			////////////////////////////////////////////////////
			//	Point Light
			////////////////////////////////////////////////////

			else if (light[i].direction.x == 0 &&
					 light[i].direction.y == 0 &&
					 light[i].direction.z == 0)
				 {	

					float3 lightDir = (light[i].position.xyz - inputPixel.WrlPos);
					
					float distance = length(lightDir);
					lightDir = lightDir / distance; //normalized direction vector
					float limit = 1 - saturate(distance / light[i].range);
					limit = limit * limit; // exponential
					
					float lightratio = saturate(dot(lightDir, inputPixel.nrm));
					
					result[i] = limit * lightratio * light[i].color * textureColor;
					
					////////////////////////////////////////////////////////////////////
					//	SPECULAR MATH
					////////////////////////////////////////////////////////////////////
					
					//VIEWDIR = NORMALIZE( CAMWORLDPOS – SURFACEPOS ) 
					//HALFVECTOR = NORMALIZE((-LIGHTDIR) + VIEWDIR)
					//INTENSITY = SATURATE(POW(DOT(NORMAL, NORMALIZE(HALFVECTOR)), SPECULARPOWER));
					//RESULT = LIGHTCOLOR * SPECULARINTENSITY * INTENSITY
					
					float3 viewDir = normalize(inputPixel.WrlPos - cameraPos);
					//float3 reflectVector = normalize((light[i].position.xyz - inputPixel.WrlPos) + viewDir);
					//float3 reflectVector = normalize(reflect((light[i].position.xyz - inputPixel.WrlPos) + viewDir, inputPixel.nrm));
					float3 reflectVector = normalize(reflect((inputPixel.WrlPos - light[i].position.xyz) + viewDir, inputPixel.nrm));
					float intensity = saturate(pow(dot(inputPixel.nrm, normalize(reflectVector)), specularPower));
					//float intensity = 0.3f;
					
					result[i] += light[i].color * specularIntensity * intensity;
					
					////////////////////////////////////////////////////////////////////
					
			 	 }







			////////////////////////////////////////////////////
			//	End of Point Light
			////////////////////////////////////////////////////
			}



		//
	}
	float3 total = float3(0,0,0);
	for (unsigned int i = 0; i < 2; i++)
	{
		total += result[i];
	}
	textureColor.xyz = total + ambientColor;


	//textureColor.xyz = result0;
	//textureColor.xyz = light.color;
	return textureColor;
}