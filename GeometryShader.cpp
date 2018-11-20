// geometry shader.cpp
#include "geometryshader.h"


GeometryShader::GeometryShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"triangle_vs.cso", L"triangle_gs.cso", L"triangle_ps.cso");
}


GeometryShader::~GeometryShader()
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
	if(blendState)
	{
		blendState->Release();
		blendState = 0;
	}
	if(billboardBuffer)
	{
		billboardBuffer->Release();
		billboardBuffer = 0;
	}
	if(cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void GeometryShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC billboardBufferDesc;
	 D3D11_BLEND_DESC blendDesc;
	 D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Load (+ compile) shader files
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
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
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

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	billboardBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	billboardBufferDesc.ByteWidth = sizeof(BillboardBufferType);
	billboardBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	billboardBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	billboardBufferDesc.MiscFlags = 0;
	billboardBufferDesc.StructureByteStride = 0;

	// https://stackoverflow.com/questions/11813698/directx-11-alpha-blending-not-working

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&billboardBufferDesc, NULL, &billboardBuffer);
	
	renderTargetBlendDesc.BlendEnable = true;
	renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.RenderTargetWriteMask = 0x0F;

	blendDesc.AlphaToCoverageEnable = FALSE;
	// setup blending for alpha transparency mapping
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0] = renderTargetBlendDesc;

	renderer->CreateBlendState(&blendDesc, &blendState);
}

void GeometryShader::initShader(WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadGeometryShader(gsFilename);
}


void GeometryShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, Camera* camera, XMFLOAT2 spriteSize, XMFLOAT4 spritePositions[8], bool cylinBillboarding, float nrOfQuads)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	CameraBufferType* camPtr;
	BillboardBufferType* billPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;
	
	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	// Unlock the constant buffer.
	deviceContext->Unmap(matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	// create the camera buffer
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	// create pointer to the data in our camera buffer
	camPtr = (CameraBufferType*)mappedResource.pData;
	// copy cam position into the pointer
	camPtr->cameraPosition = camera->getPosition();
	camPtr->padding1 = 0.0f;
	// calculate the up vector
	camPtr->cameraUp = camera->getCameraUp();
	camPtr->padding2 = 0.0f;
	// unlock cam buffer
	deviceContext->Unmap(cameraBuffer, 0);

	bufferNumber = 1;

	// Now set the constant buffer in the geometry shader with the updated values.
	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &cameraBuffer);

	// create the camera buffer
	deviceContext->Map(billboardBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	// create pointer to the data in our camera buffer
	billPtr = (BillboardBufferType*)mappedResource.pData;
	billPtr->spriteDimensions = spriteSize;
	for(int i = 0; i < 8; i++)
	{
		// populate position arrays
		billPtr->spritePositions[i] = spritePositions[i];
	}
	billPtr->cylindricalBillboarding = cylinBillboarding;
	billPtr->nrOfQuads = nrOfQuads;
	// unlock cam buffer
	deviceContext->Unmap(billboardBuffer, 0);

	bufferNumber = 2;

	// Now set the constant buffer in the geometry shader with the updated values.
	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &billboardBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
}

void GeometryShader::toggleBlendState(ID3D11DeviceContext* deviceContext, bool shouldBlend)
{
	if(shouldBlend)
	{
		deviceContext->OMSetBlendState(blendState, NULL, 0xFFFFFF);	
	}
	else
	{
		// set it to the default blend vallue
		deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFF);
	}
}

void GeometryShader::render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	// Base render function.
	BaseShader::render(deviceContext, indexCount);
}



