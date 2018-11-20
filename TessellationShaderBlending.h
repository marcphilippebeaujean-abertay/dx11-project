// Light shader.h
// Basic single light shader setup
#ifndef _TESSELLATIONBLENDERSHADER_H_
#define _TESSELLATIONBLENDERSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Camera.h"
#include "MyLight.h"
#include <vector>

using namespace std;
using namespace DirectX;

#define NROFLIGHTS 3


class TessellationBlendingShader : public BaseShader
{
public:
	TessellationBlendingShader(ID3D11Device* device, HWND hwnd);
	~TessellationBlendingShader();

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	struct TessellationFactorType
	{
		float minDist;
		float maxDist;
		float minTess;
		float maxTess;
	};

	struct LightStruct
	{
		XMFLOAT4 diffuseColour;
		XMFLOAT3 lightPosition;
		float lightRadius;
		XMFLOAT3 lightLookAt;
		float specularPower;
		XMFLOAT4 specularColour;
		float lightType;
		float spotLightInnerAngle;
		float spotLightOutterAngle;
		bool usesSpecMap;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColour;
		LightStruct myLights[NROFLIGHTS];
	};

	struct DisplacementBufferType
	{
		float heightMod;
		XMFLOAT3 padding;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct FogBufferType
	{
		XMFLOAT3 fogColour;
		float fogRange;
		bool fogActive;
		float fogStart;
		float maxFogAmnt;
		float padding;
	};

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float displacementMod, ID3D11ShaderResourceView* heightMap, Camera* camera, bool usesSpecMap, ID3D11ShaderResourceView* specMap, std::vector<MyLight*> lights, ID3D11ShaderResourceView* shadowMap, XMFLOAT4 tessellationFactors, ID3D11ShaderResourceView* alphaMap, ID3D11ShaderResourceView* blendTexture);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);
	void setFogParameters(ID3D11DeviceContext* deviceContext, bool activeFog, float fogStart, float fogRange, float maxFogAmnt, XMFLOAT3 fogColour);
private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);
private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* displacementBuffer;
	ID3D11Buffer* fogBuffer;
	ID3D11Buffer* tessellationFactorBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateClamp;
};

#endif