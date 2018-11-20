// tessellation shader.cpp
#include "tessellationshader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"tessellation_vs.cso", L"tessellation_hs.cso", L"tessellation_ds.cso", L"tessellation_ps.cso");
}


TessellationShader::~TessellationShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	if (displacementBuffer)
	{
		displacementBuffer->Release();
		displacementBuffer = 0;
	}

	if (fogBuffer)
	{
		fogBuffer->Release();
		fogBuffer = 0;
	}


	//Release base shader components
	BaseShader::~BaseShader();
}

void TessellationShader::initShader(WCHAR* vsFilename,  WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC displacementBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC fogBufferDesc;
	D3D11_BUFFER_DESC tessellationBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Load shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Required a CLAMPED sampler for sampling the depth map
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleStateClamp);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	// Setup camera buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	// Setup displacement buffer
	displacementBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	displacementBufferDesc.ByteWidth = sizeof(DisplacementBufferType);
	displacementBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	displacementBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	displacementBufferDesc.MiscFlags = 0;
	displacementBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&displacementBufferDesc, NULL, &displacementBuffer);

	// Setup fog buffer
	fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogBufferDesc.ByteWidth = sizeof(FogBufferType);
	fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogBufferDesc.MiscFlags = 0;
	fogBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&fogBufferDesc, NULL, &fogBuffer);

	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationFactorType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&tessellationBufferDesc, NULL, &tessellationFactorBuffer);

}

void TessellationShader::initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, float displacementMod, ID3D11ShaderResourceView* heightMap, Camera* camera, bool usesSpecMap, ID3D11ShaderResourceView* specMap, std::vector<MyLight*> lights, ID3D11ShaderResourceView* shadowMap, XMFLOAT4 tessellationFactors)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	DisplacementBufferType* dispPtr;
	CameraBufferType* cameraPtr;
	LightBufferType* lightPtr;
	TessellationFactorType* tessPtr;

	//TessellationBufferType* tessellationPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj, lview, lproj;

	// Set buffer nr to 0
	bufferNumber = 0;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	lview = XMMatrixTranspose(lights[0]->getViewMatrix());
	lproj = XMMatrixTranspose(lights[0]->getOrthoMatrix());
	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightProjection = lproj;
	dataPtr->lightView = lview;
	// Unlock the constant buffer.
	deviceContext->Unmap(matrixBuffer, 0);

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	// Send light data to pixel shader
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambientColour = lights[0]->getAmbientColour();
	for (int i = 0; i < lights.size(); i++)
	{
		lightPtr->myLights[i].diffuseColour = lights[i]->getDiffuseColour();
		lightPtr->myLights[i].lightPosition = lights[i]->getPosition();
		lightPtr->myLights[i].specularColour = lights[i]->getSpecularColour();
		lightPtr->myLights[i].specularPower = lights[i]->getSpecularPower();
		lightPtr->myLights[i].lightLookAt = lights[i]->getDirection();
		lightPtr->myLights[i].lightRadius = lights[i]->getRadius();
		lightPtr->myLights[i].lightType = static_cast<float>(lights[i]->getLightType());
		lightPtr->myLights[i].usesSpecMap = usesSpecMap;
		if (lightPtr->myLights[i].lightType > 1.5)
		{
			lightPtr->myLights[i].spotLightInnerAngle = lights[i]->getSpotInnerAngle();
			lightPtr->myLights[i].spotLightOutterAngle = lights[i]->getSpotOutterAngle();
		}
	}
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);

	// Set buffer nr to 1
	bufferNumber = 1;

	// setup displacement buffer
	deviceContext->Map(displacementBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dispPtr = (DisplacementBufferType*)mappedResource.pData;
	dispPtr->heightMod = displacementMod;
	dispPtr->padding = XMFLOAT3(0, 0, 0);
	// unmap buffer
	deviceContext->Unmap(displacementBuffer, 0);
	// set correct buffer nr and set buffer
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &displacementBuffer);

	// setup displacement buffer
	deviceContext->Map(tessellationFactorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessellationFactorType*)mappedResource.pData;
	tessPtr->maxDist = tessellationFactors.x;
	tessPtr->minDist = tessellationFactors.y;
	tessPtr->maxTess = tessellationFactors.z;
	tessPtr->minTess = tessellationFactors.w;
	// unmap buffer
	deviceContext->Unmap(tessellationFactorBuffer, 0);
	// set correct buffer nr and set buffer
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &tessellationFactorBuffer);


	// Set buffer nr to 2s
	bufferNumber = 2;

	//Additional
	// lock camera buffer
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	// create pointer to the data in our camera buffer
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	// copy cam position into the pointer
	cameraPtr->cameraPosition = camera->getPosition();
	cameraPtr->padding = 0.0f;
	// unlock cam buffer
	deviceContext->Unmap(cameraBuffer, 0);

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &cameraBuffer);
	deviceContext->HSSetConstantBuffers(0, 1, &cameraBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &specMap);
	deviceContext->PSSetShaderResources(2, 1, &shadowMap);
	deviceContext->DSSetShaderResources(0, 1, &heightMap);
}

void TessellationShader::setFogParameters(ID3D11DeviceContext* deviceContext, bool activeFog, float fogStart, float fogRange, float maxFogAmnt, XMFLOAT3 fogColour)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	FogBufferType* fogPtr;
	unsigned int bufferNumber = 1;
	// Send fog data to pixel shader
	result = deviceContext->Map(fogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	fogPtr = (FogBufferType*)mappedResource.pData;
	fogPtr->fogActive = activeFog;
	fogPtr->fogStart = fogStart;
	fogPtr->fogRange = fogRange;
	fogPtr->fogColour = fogColour;
	fogPtr->maxFogAmnt = maxFogAmnt;
	deviceContext->Unmap(fogBuffer, 0);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &fogBuffer);
}

void TessellationShader::render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->DSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateClamp);

	// Base render function.
	BaseShader::render(deviceContext, indexCount);
}



