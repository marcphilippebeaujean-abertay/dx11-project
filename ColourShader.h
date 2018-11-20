// Colour shader.h
// Simple shader example.
#ifndef _COLOURSHADER_H_
#define _COLOURSHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class ColourShader : public BaseShader
{

public:

	struct MorphBufferType
	{
		float deltaTime;
		float morphHeight;
		float morphFrequency;
		float morphSpeed;
		float morphHeightMod;
		bool shouldMorph;
		XMFLOAT2 padding;
	};

	ColourShader(ID3D11Device* device, HWND hwnd);
	~ColourShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection);
	void setMorphParameters(ID3D11DeviceContext* deviceContext, float DT, float height, float frequency, float speed, float heightMod, bool toggleMorph);
private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* morphBuffer;
};

#endif