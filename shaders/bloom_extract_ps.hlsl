// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

cbuffer ThresholdBuffer : register(b0)
{
    float bloomThreshold;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    // sample the pixel color from the texture using the sampler at this texture coordinate location
    textureColor = texture0.Sample(Sampler0, input.tex);
    // extract the bright colours from our scene using the bloom threshold
    return saturate((textureColor - bloomThreshold) / (1 - bloomThreshold));
}