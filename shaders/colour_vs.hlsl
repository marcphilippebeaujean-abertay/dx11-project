// colour vertex shader
// Simple geometry pass
// texture coordinates and normals will be ignored.

cbuffer MatrixBuffer : register(cb0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer MorphBuffer : register(b1)
{
	float deltaTime;
	float morphHeight;
	float morphFrequency;
	float morphSpeed;
	float morphHeightMod;
	bool shouldMorph;
	float2 padding;
};

struct InputType
{
	float4 position : POSITION;
};

struct OutputType
{
	float4 position : SV_POSITION;
};

OutputType main(InputType input)
{
	OutputType output;
	
	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	if (shouldMorph)
	{
		//offset position based on sine wave - hard coded, because for some reason these values aren't passed through the buffer for some reason
		input.position.y += morphHeight * morphHeightMod * sin((morphFrequency * input.position.x) + (deltaTime * morphSpeed));
		input.position.x += morphHeight * morphHeightMod * sin((morphFrequency * input.position.y) + (deltaTime * morphSpeed));
	}

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	return output;
}