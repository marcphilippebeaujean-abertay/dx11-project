#ifndef _VERBLURSHADER_H_
#define _VERBLURSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Camera.h"

class BlurShaderVer : public BaseShader
{
public:
	BlurShaderVer(ID3D11Device* device, HWND hwnd);
	~BlurShaderVer();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float screen_height);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR*, WCHAR*);
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* screenBuffer;
	ID3D11SamplerState* sampleState;

	struct ScreenBufferType
	{
		float screen_height;
		XMFLOAT3 padding;
	};
};

#endif