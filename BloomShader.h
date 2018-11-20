#pragma once
#include "..\DXFramework\BaseShader.h"
class BloomShader :
	public BaseShader
{
public:
	BloomShader(ID3D11Device* device, HWND hwnd);
	~BloomShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* glossMap, ID3D11ShaderResourceView* sceneRender);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR*, WCHAR*);
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};

