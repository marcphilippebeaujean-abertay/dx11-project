#pragma once

#include "../DXFramework/BaseMesh.h"

class TessellationPlane : public BaseMesh
{
public:
	TessellationPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 100);
	~TessellationPlane();

	void sendData(ID3D11DeviceContext*) override;
	int resolution;

private:
	
	void initBuffers(ID3D11Device* device);
};

