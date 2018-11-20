// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

cbuffer InversionBuffer : register (cb0)
{
	float invertColour;
	float3 dispPadding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
	
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = texture0.Sample(Sampler0, input.tex);

	if (invertColour > 0)
	{
		textureColor = (1 - textureColor); // Colors range from 0 to 1, subtract original value to get inversion
	}

    return textureColor;
}