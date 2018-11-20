#include "SceneObject.h"


SceneObject::SceneObject(ID3D11ShaderResourceView* diffuseTexture, ID3D11ShaderResourceView* normalMap, ID3D11ShaderResourceView* displacementMap, ID3D11ShaderResourceView* glossMap, BaseMesh* renderMesh, XMMATRIX worldMatrix)
{
	// inititialise variables
	this->diffuseTexture = diffuseTexture;
	this->displacementMap = displacementMap;
	this->normalMap = normalMap;
	this->glossMap = glossMap;
	this->renderMesh = renderMesh;
	this->worldMatrix = worldMatrix;
}

SceneObject::~SceneObject()
{
}
