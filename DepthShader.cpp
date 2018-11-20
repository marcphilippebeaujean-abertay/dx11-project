// depth shader.cpp
#include "depthshader.h"


DepthShader::DepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"depth_vs.cso", L"depth_hs.cso", L"depth_ds.cso", L"depth_ps.cso");
}


DepthShader::~DepthShader()
{
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

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthShader::initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void DepthShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC displacementBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC tessellationBufferDesc;

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

	// Setup displacement buffer
	displacementBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	displacementBufferDesc.ByteWidth = sizeof(DisplacementBufferType);
	displacementBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	displacementBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	displacementBufferDesc.MiscFlags = 0;
	displacementBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&displacementBufferDesc, NULL, &displacementBuffer);

	// Setup camera buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationFactorType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&tessellationBufferDesc, NULL, &tessellationFactorBuffer);
}


void DepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, float displAmnt, ID3D11ShaderResourceView* heightMap, Camera* camera, XMFLOAT4 tessellationFactors)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	CameraBufferType* cameraPtr;
	DisplacementBufferType* dispPtr;
	TessellationFactorType* tessPtr;
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
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

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

	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &cameraBuffer);

	bufferNumber = 1;

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

	// setup displacement buffer
	deviceContext->Map(displacementBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dispPtr = (DisplacementBufferType*)mappedResource.pData;
	dispPtr->heightMod = displAmnt;
	dispPtr->padding = XMFLOAT3(0, 0, 0);
	// unmap buffer
	deviceContext->Unmap(displacementBuffer, 0);
	// set correct buffer nr and set buffer
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &displacementBuffer);

	// setup height map
	deviceContext->DSSetShaderResources(0, 1, &heightMap);
}

void DepthShader::render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the domain shader.
	deviceContext->DSSetSamplers(0, 1, &sampleState);

	// Base render function.
	BaseShader::render(deviceContext, indexCount);
}

