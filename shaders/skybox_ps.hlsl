// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

TextureCube tex_cube : register(t0);
SamplerState trilinear_sampler : register(s0)
{
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct InputType
{
	float4 position : SV_POSITION;
	float3 tex : TEXCOORD0;
};


float4 main(InputType input) : SV_TARGET
{
	return tex_cube.Sample(trilinear_sampler, input.tex);
}