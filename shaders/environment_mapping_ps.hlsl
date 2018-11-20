// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
TextureCube environmentTexture : register(t1);
SamplerState SampleType : register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
};

cbuffer ReflectionBuffer : register(b0)
{
	float4 ambient_colour;
	float4 environment_colour;
	float3 padding;
	float reflection_amnt;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 reflection_vec : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
	// final colour of pixel that will be sent to output
	float4 final_colour = (float4)0;
	// sample the base texture of the object that will be reflected upon
	float4 texture_colour = shaderTexture.Sample(SampleType, input.tex);
	// calculate the ambient colour
	float3 ambient = ambient_colour * texture_colour.xyz;
	// grab the colour information from the cube map
	float3 environment = environmentTexture.Sample(SampleType, input.reflection_vec).xyz;
	// calculate the basic colour vector for our reflection
	float3 reflection = environment_colour * environment;
	// finally, linerally interpolate between the amount of reflection that we want and the various colour vectors
	final_colour.xyz = lerp(ambient, reflection, reflection_amnt);
	final_colour.w = 1.0f;
	return final_colour;
}

