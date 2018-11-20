// Light shader.h
// Basic single light shader setup
#ifndef _ENVREFLSHADER_H_
#define _ENVREFLSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"
#include "../DXFramework/Camera.h"
#include <vector>

using namespace std;
using namespace DirectX;

#define nr_of_lights 2

class EnvironmentMapShader : public BaseShader
{
private:
	struct ReflectionBufferType
	{
		XMFLOAT4 ambient_colour;
		XMFLOAT4 environment_colour;
		XMFLOAT3 padding;
		float reflection_amnt;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

public:

	EnvironmentMapShader(ID3D11Device* device, HWND hwnd);
	~EnvironmentMapShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* environment, Camera* cam, XMFLOAT4 ambient_colour, XMFLOAT4 environment_colour, float reflection_amnt);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* reflectionBuffer;
	ID3D11Buffer* cameraBuffer;
};

#endif