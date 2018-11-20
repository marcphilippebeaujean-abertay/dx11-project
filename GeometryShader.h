// Light shader.h
// Geometry shader example.
#ifndef _GEOMETRYSHADER_H_
#define _GEOMETRYSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Camera.h"

using namespace std;
using namespace DirectX;


class GeometryShader : public BaseShader
{

public:

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding1;
		XMVECTOR cameraUp;
		float padding2;
	};

	struct BillboardBufferType
	{
		XMFLOAT2 spriteDimensions;
		bool cylindricalBillboarding;
		float nrOfQuads;
		XMFLOAT4 spritePositions[8];
	};

	GeometryShader(ID3D11Device* device, HWND hwnd);
	~GeometryShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Camera* camera, XMFLOAT2 spriteSize, XMFLOAT4 spritePositions[8], bool cylinBillboarding, float nrOfQuads);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);
	void toggleBlendState(ID3D11DeviceContext* deviceContext, bool shouldBlend);
private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* billboardBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11BlendState* blendState;
};

#endif