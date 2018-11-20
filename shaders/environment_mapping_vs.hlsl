// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b1)
{
	float3 cameraPosition;
	float padding;
}

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 reflection_vec : TEXCOORD1;
};

OutputType main(InputType input)
{
	OutputType output;

	float3 worldPosition;
	float3 incident;
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
	input.normal = mul(input.normal, (float3x3)worldMatrix);

	// Normalize the normal vector.
	input.normal = normalize(input.normal);

	// Calculate the position of the vertex in the world.
	worldPosition = mul(input.position, worldMatrix);

	// Determine the incident used for reflection calculations
	incident = normalize(worldPosition - cameraPosition);
	
	// Finally, calculate the reflection vector with the "reflection" insinctric - formula is R = I - 2 * N * (I . N)
	output.reflection_vec = reflect(incident, input.normal);

	return output;
}