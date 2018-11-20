#pragma once

#include "../DXFramework/DXF.h"
class SceneObject
{
public:
	SceneObject(ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* normalMap, ID3D11ShaderResourceView* displacementMap, ID3D11ShaderResourceView* glossMap, BaseMesh* renderMesh, XMMATRIX worldMatrix);
	~SceneObject();

	// setters
	void setWorldMatrix(XMMATRIX newWorldMatrix) { worldMatrix = newWorldMatrix; }
	
	// getters
	XMMATRIX getWorldMatrix() { return worldMatrix; }
	ID3D11ShaderResourceView* getDiffuseTexture() { return diffuseTexture; };
	ID3D11ShaderResourceView* getNormalMap() { return normalMap; }
	ID3D11ShaderResourceView* getDisplacementMap() { return displacementMap; }
	ID3D11ShaderResourceView* getGlossMap() { return glossMap; }
	BaseMesh* getMesh() { return renderMesh; }

private:
	BaseMesh* renderMesh;
	ID3D11ShaderResourceView* diffuseTexture;
	ID3D11ShaderResourceView* normalMap;
	ID3D11ShaderResourceView* displacementMap;
	ID3D11ShaderResourceView* glossMap;
	XMMATRIX worldMatrix;
};

