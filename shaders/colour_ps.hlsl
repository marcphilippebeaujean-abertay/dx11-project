// Colour pixel/fragment shader
// Basic fragment shader outputting a colour

struct InputType
{
	float4 position : SV_POSITION;
};


float4 main(InputType input) : SV_TARGET
{
	float4 colour = float4(1.0, 0.0, 0.0, 1.0);

	return colour;
}