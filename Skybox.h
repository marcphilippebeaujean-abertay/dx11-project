// http://iloveshaders.blogspot.co.uk/2011/05/creating-sky-box.html
// https://msdn.microsoft.com/en-us/library/bb464016.aspx
#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;

class Skybox : public BaseShader
{

public:

	Skybox(ID3D11Device* device, HWND hwnd);
	~Skybox();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);
	void toggleCulling(ID3D11DeviceContext* deviceContext, bool cullingActive);
private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11RasterizerState* rasterState;
};

#endif