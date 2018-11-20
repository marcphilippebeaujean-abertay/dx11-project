// Colour shader.h
// Simple shader example.
#ifndef _DEPTHSHADER_H_
#define _DEPTHSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Camera.h"

using namespace std;
using namespace DirectX;


class DepthShader : public BaseShader
{

public:

	struct TessellationFactorType
	{
		float minDist;
		float maxDist;
		float minTess;
		float maxTess;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct DisplacementBufferType
	{
		float heightMod;
		XMFLOAT3 padding;
	};

	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, float displAmnt, ID3D11ShaderResourceView* heightMap, Camera* camera, XMFLOAT4 tessFactors);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);
private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* displacementBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* tessellationFactorBuffer;
};

#endif