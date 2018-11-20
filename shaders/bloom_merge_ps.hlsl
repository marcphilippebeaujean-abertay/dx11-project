// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry
Texture2D glossMap : register(t0);
Texture2D sceneRender : register(t1);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 colour;
    // sample the pixel color from the texture using the sampler at this texture coordinate location
    float4 gloss = glossMap.Sample(Sampler0, input.tex);
    float4 sceneColour = sceneRender.Sample(Sampler0, input.tex);
    // extract bloom colours from the original scene
    sceneColour *= (1 - saturate(gloss));
    // combine bloom and scene colour
    colour = (sceneColour + gloss);
    return colour;
}