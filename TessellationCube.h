#pragma once
#include "../DXFramework/BaseMesh.h"

class TessellationCube : public BaseMesh
{
public:
	TessellationCube(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 20);
	~TessellationCube();

	void sendData(ID3D11DeviceContext*) override;
	int resolution;

private:

	void initBuffers(ID3D11Device* device);
};

