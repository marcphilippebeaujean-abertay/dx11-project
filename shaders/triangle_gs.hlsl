// Example geometry shader
// Receives a point and outputs a triangle

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b1)
{
	float3 cameraPosition;
	float padding1;
	vector cameraUp;
	float padding2;
};

cbuffer BillboardBuffer : register(b2)
{
    float2 spriteDimensions;
	bool cylindricalBillboarding;
	float nrOfQuads;
    float4 spritePositions[8];
}

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

// pixel input type
struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

cbuffer QuadTexCoords
{
	static float2 uv_positions[4] =
	{
		float2(1, 0),
		float2(1, 1),
		float2(0, 0),
		float2(0, 1)
	};
};

// gs function
[maxvertexcount(36)]
void main(point InputType input[1], inout TriangleStream<OutputType> triStream)
{
    OutputType output;

	// find the world position of the vertex for our direction calculations
	float4 vertexWorldPosition = mul(float4(input[0].position.xyz, 1.0f), worldMatrix);
	// calculate direction vector from the center point (input position) of the quad and the camera position
    float3 direction = cameraPosition - vertexWorldPosition;
	// calculate the orthographic surface vector of the quad we are creating
    float3 orthoSurfaceVec = cross(normalize(direction), cameraUp.xyz);
    // determine the dimensions of the newly created quads
    float2 halfSize = spriteDimensions / 2;
    // create the x offset using the newly generate ortho vector, making the quad face the camera
    float3 offsetX = halfSize.x * orthoSurfaceVec;
	float3 offsetY;
	if (cylindricalBillboarding)
	{
		// generate a constant "up" direction so to lock the yaw rotation
		offsetY = halfSize.y * float3(0, 1, 0);
	}
	else
	{
		// create the y offset based on the camera up, meaning the yaw will rotate towards the camera
		offsetY = halfSize.y * cameraUp;
	}
	// create quad with the geometry shader
    float4 vertices[4];
    // create the lower left vertex
    vertices[0] = float4(input[0].position.xyz - offsetX + offsetY, 1.0f);
    // create the upper left vertex
    vertices[1] = float4(input[0].position.xyz - offsetX - offsetY, 1.0f);
    // create the upper right vertex
    vertices[2] = float4(input[0].position.xyz + offsetX + offsetY, 1.0f);
    // create the lower right vertex
    vertices[3] = float4(input[0].position.xyz + offsetX - offsetY, 1.0f);
    // create outter for loop that helps spread out the billboards
    for (int x = 0; x < nrOfQuads; x++)
    {
        // render each vertex
        for (int i = 0; i < 4; i++)
        {
            // translate vertex
            output.position = mul(float4((vertices[i].xyz + spritePositions[x].xyz), 1.0f), worldMatrix);
            output.position = mul(output.position, viewMatrix);
            output.position = mul(output.position, projectionMatrix);
            // take the corresponding texture coordinate
            output.tex = uv_positions[i];
            // output vertex to the pixel shader
            triStream.Append(output);
        }
        // restart the strip to create another quad
        triStream.RestartStrip();
    }
}