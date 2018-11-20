#pragma once
#include "..\DXFramework\BaseShader.h"
class GlossExtractShader :
	public BaseShader
{
public:

	struct ThresholdBufferType
	{
		float bloomThreshold;
		XMFLOAT3 padding;
	};

	GlossExtractShader(ID3D11Device* device, HWND hwnd);
	~GlossExtractShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float bloomThreshold);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR*, WCHAR*);
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* thresholdBuffer;
	ID3D11SamplerState* sampleState;
};

