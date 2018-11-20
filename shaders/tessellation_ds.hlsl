// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D heightMap : register(t0);
SamplerState SampleType : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer DisplacementBuffer : register (b1)
{
	float heightMod;
	float3 dispPadding;
};

cbuffer CameraBuffer : register(b2)
{
	float3 cameraPosition;
	float camPadding;
}

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 position3D : TEXCOORD1;
	float3 viewDirection: TEXCOORD2;
	float distanceToCam : TEXCOORD3;
	float4 lightViewPosition : TEXCOORD4;
};

[domain("tri")]
OutputType main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 3> patch)
{
    float3 vertexPosition;
    OutputType output;
	float4 worldPosition;
	
	// store the texture coordinates
	output.tex = patch[0].tex * uvwCoord.x + patch[1].tex * uvwCoord.y + patch[2].tex * uvwCoord.z;

    // Determine the position of the new vertex.
	// Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
	// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).
	vertexPosition = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;

	// calculate the normal against the world matrix
	output.normal = patch[0].normal * uvwCoord.x + patch[1].normal * uvwCoord.y + patch[2].normal * uvwCoord.z;
	// translate the normals by the objects world position
	output.normal = mul(output.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	if (heightMod > 0.0f)
	{
		// get the displacement information from the texture
		float4 displacementMap = heightMap.SampleLevel(SampleType, output.tex, 0);
		// calculat the amount by which the height map effects the vertex
		float heightValue = (heightMod * displacementMap.y);
		// apply height map to the vertex position - multiplying by the normal means it will manipulate the right side
		vertexPosition += output.normal * heightValue;
	}
    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPosition = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	// Calculate the position of the vertex in the world.
	worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix);

	// Determine the distance from the camera to the vertex
	output.distanceToCam = distance(worldPosition, cameraPosition);

	// Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// Normalize the viewing direction vector.
	output.viewDirection = normalize(output.viewDirection);

	// position in 3D space for lighting
	output.position3D = mul(float4(vertexPosition, 1.0f), worldMatrix);

    return output;
}

