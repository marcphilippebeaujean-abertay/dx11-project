// Tessellation pixel shader
// Output colour passed to stage.
Texture2D shaderTexture : register(t0);
Texture2D specularMap : register(t1);
Texture2D shadowMap : register(t2);
Texture2D alphaMap : register(t3);
Texture2D blendTexture : register(t4);
SamplerState SampleType : register(s0);
SamplerState SampleTypeClamp : register(s1);

#define NROFLIGHTS 3

struct LightStruct
{
	float4 diffuseColour;
	float3 lightPosition;
	float lightRadius;
	float3 lookAt;
	float specularPower;
	float4 specularColour;
	float lightType;
	float spotLightInnerAngle;
	float spotLightOutterAngle;
	bool usesSpecMap;
};

cbuffer LightBuffer : register(b0)
{
	float4 ambientColour;
	LightStruct myLights[NROFLIGHTS];
};

cbuffer FogBuffer : register (b1)
{
	float3 fogColour;
	float fogRange;
	bool fogActive;
	float fogStart;
    float maxFogAmnt;
	float padding;
}


struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 position3D : TEXCOORD1;
	float3 viewDirection: TEXCOORD2;
	float distanceToCam : TEXCOORD3;
	float4 lightViewPosition : TEXCOORD4;
};

float4 main(InputType input) : SV_TARGET
{
	// final colour of pixel that will be sent to output
	float4 finalColour = (float4)0;
	// adds all the light colours together before final calculation
	float3 finalLightColour = float3(0, 0, 0);
	// calculate the amount of fog on this pixel, if fog is enabled
    float fogAmnt = clamp((input.distanceToCam - fogStart), 0.0f, fogRange);
    if(fogAmnt > 0)
    {
        fogAmnt = saturate(clamp(fogAmnt / fogRange, 0.0f, maxFogAmnt));
    }
	// sample the pixel colour from the texture.
    float alphaValue = saturate(alphaMap.SampleLevel(SampleType, input.tex, 0).x);
	float4 textureColour = shaderTexture.Sample(SampleType, input.tex);
    float4 blendColour = blendTexture.Sample(SampleType, input.tex);
    // interpolate between the two textures
    textureColour.xyz *= alphaValue;
    blendColour.xyz *= 1 - alphaValue;
    textureColour.xyz += blendColour.xyz;

	// calculate the ambient colour
	float3 ambient = ambientColour * textureColour;
	
	// SHADOW MAP DEPENDANCIES

	bool inShadow = true;
	float4 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	// Set the bias value for fixing the floating point precision issues.
	const float bias = 0.0008f;
	// Calculate prjected coordinates, then into UV range
	projectTexCoord.xyz = input.lightViewPosition.xyz / input.lightViewPosition.w;

	// Calculate the projected texture coordinates.
	projectTexCoord.x = (projectTexCoord.x / 2.0f) + 0.5f;
	projectTexCoord.y = (-projectTexCoord.y / 2.0f) + 0.5f;

	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = shadowMap.Sample(SampleTypeClamp, projectTexCoord).r;

		// Calculate the depth of the light.
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if (lightDepthValue < depthValue)
		{
			inShadow = false;
		}
	}
	for (int i = 0; i < NROFLIGHTS; i++)
	{
		// the light direction sent by the buffer, which specifies the direction that the light is facing - used for directional lights and spot lights
		float3 lightRotation = normalize(-myLights[i].lookAt);
		// variables exclusive to non-directional lights
		float3 lightDir;
		float attenuation;
		// 0 = directional, 1 = point, 2 = spot
		if (myLights[i].lightType < 0.5f)
		{
			// for directional lights, it is predetermined where the light will come from (and attenuation won't exist either)
			lightDir = lightRotation;
			attenuation = 1;
		}
		else
		{
			// init setup for light direction when it's not a directional light
			lightDir = (myLights[i].lightPosition - input.position3D);
			// calculate attenuation
			attenuation = saturate(1.0f - (length(lightDir) / myLights[i].lightRadius));
			lightDir = normalize(lightDir); // normalise the light direction AFTER making attenuation calculations
		}
		// Calculate the amount of light on this pixel.
		float lightIntensity = saturate(dot(input.normal, lightDir));
		if (lightIntensity > 0)
		{
			// variable used for spot light calculations
			float spotFactor = 1.0f;
			if (myLights[i].lightType > 1.5f)
			{
				// calculate the angle of the spotlight
				float lightAngle = dot(lightRotation, lightDir);
				if (lightAngle > 0)
				{
					spotFactor = smoothstep(myLights[i].spotLightInnerAngle, myLights[i].spotLightOutterAngle, lightAngle);
				}
			}
			// calc half_vec for Blinn-Phong calculations
			float3 halfVec = normalize(lightDir + input.viewDirection);
			//// the dot product of the half-angle vector and the surface normal - used for specular calculation
			float n_dot_halfway = dot(input.normal, halfVec);
			// calculate the diffuse lighting
			float3 diffuse = myLights[i].diffuseColour * lightIntensity;
			//// calculate the specular lighting
			float glossSample = 1.0f;
			if (myLights[i].usesSpecMap)
			{
				// store the specular colour information from the texture (which parts should be specular and which shouldn't)
				float4 specularMapSample = specularMap.SampleLevel(SampleType, input.tex, 0);
				// since the rgb values will be either 1 or 0, it doesn't matter which axis we choose
				glossSample = specularMapSample.x;
			}
			float spec = pow(min(n_dot_halfway, textureColour.w), myLights[i].specularPower);
			float3 specular = saturate(myLights[i].specularColour * glossSample  * spec);
			// calculate the final light value by adding the effects together
			float3 colourCombination = (spotFactor * (diffuse + specular)  * attenuation);
			// cast shadows using the directional light
			if (myLights[i].lightType < 0.5f)
			{
                if (inShadow)
                {
                    colourCombination = ambientColour;
                }
            }
			finalLightColour += colourCombination;
		}
	}
	// calculate the final colour value
	finalColour.xyz = (ambient + finalLightColour);
	if (fogActive)
	{
		// calculate the effect of the fog on the colour (if fog is enabled)
		finalColour.xyz = lerp(finalColour.xyz, fogColour, fogAmnt);
	}
	finalColour.w = 1.0f;
	return finalColour;
}