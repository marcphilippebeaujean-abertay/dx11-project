#pragma once
#include "../DXFramework/BaseMesh.h"

class TessellationSphere : public BaseMesh
{
public:
	TessellationSphere(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 20);
	~TessellationSphere();

	void sendData(ID3D11DeviceContext*) override;
	int resolution;

private:

	void initBuffers(ID3D11Device* device);
};

