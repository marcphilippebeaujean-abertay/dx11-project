// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D heightMap : register(t0);
SamplerState SampleType : register(s0);

cbuffer MatrixBuffer : register(cb0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer DisplacementBuffer : register (b1)
{
	float heightMod;
	float3 dispPadding;
};

struct ConstantOutputType
{
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

[domain("tri")]
OutputType main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	float2 textureCoords;
	OutputType output;
	
	// Translate vertex with uvwCoords
	vertexPosition = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;

	// Translate the texture coordinates with uvwCoords
	textureCoords = patch[0].tex * uvwCoord.x + patch[1].tex * uvwCoord.y + patch[2].tex * uvwCoord.z;

	if (heightMod > 0.0f)
	{
		// get the displacement information from the texture
		float4 displacementMap = heightMap.SampleLevel(SampleType, textureCoords, 0);
		// calculat the amount by which the height map effects the vertex
		float heightValue = (heightMod * displacementMap.y);
		// apply height map to the vertex position
		vertexPosition.y += heightValue;
	}

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the position value in a second input value for depth value calculations.
	output.depthPosition = output.position;

	return output;
}