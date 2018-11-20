// Tessellation Hull Shader
// Prepares control points for tessellation
Texture2D depthMap : register(t0);
SamplerState SampleType : register(s0);

cbuffer CameraBuffer : register(b0)
{
	float3 camPosition;
	float padding;
}

cbuffer TessellationFactorBuffer : register(b1)
{
    float minDist;
    float maxDist;
    float minTess;
    float maxTess;
}

// http://victorbush.com/2015/01/tessellated-terrain/

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;
    // get the distance from the midpoint of the patch to the camera
	float distanceToCam = distance(inputPatch[0].position, camPosition);
    // saturate that distance while dividing it by (max - min) to get higher tessellation the closer we are to the object
    float interpAmount = saturate((distanceToCam - minDist) / (maxDist - minDist));
    float tessFactor = pow(2, lerp(maxTess, minTess, interpAmount));

    // Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = tessFactor;
	output.edges[1] = tessFactor;
	output.edges[2] = tessFactor;

    // Set the tessellation factor for tessallating inside the triangle.
	output.inside = tessFactor;

    return output;
}


[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;
	
	// pass texture coordinates
	output.tex = patch[pointId].tex;

	// pass the normal coordinates
	output.normal = normalize(patch[pointId].normal);

    return output;
}