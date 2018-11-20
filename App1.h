// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "ColourShader.h"
#include "TessellationShader.h"
#include "TessellationPlane.h"
#include "Skybox.h"
#include "EnvironmentMapShader.h"
#include "BlurShaderHor.h"
#include "BlurShaderVer.h"
#include "TextureShader.h"
#include "TessellationShaderBlending.h"
#include "TessellationCube.h"
#include "TessellationSphere.h"
#include "DepthShader.h"
#include "GeometryShader.h"
#include "GlossExtractShader.h"
#include "BloomShader.h"
#include "SceneObject.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in);

	bool frame();

protected:
	bool render();
	void gui();

private:
	// meshes
	TessellationPlane* mesh;
	TessellationCube* cubeMesh;
	TessellationSphere* sphereMesh;
	Model* teaCup;
	float rotationZ = 0.0f;
	std::vector<BaseMesh*> tessMeshes;
	std::vector<XMMATRIX> translationMatrices;
	SphereMesh* skyMesh;
	SphereMesh* reflectionMesh;
	PointMesh* pointMesh;
	SceneObject* testSceneObject;


	// shaders
	ColourShader* colourShader;
	TessellationShader* tessellationShader;
	TessellationBlendingShader* tessellationBlendingShader;
	Skybox* skyboxShader;
	EnvironmentMapShader* environmentMapShader;
	GeometryShader* billboardingShader;
	
	std::vector<MyLight*> lights;

	// blur dependancies
	ID3D11RenderTargetView* blurRenderTarget;
	RenderTexture* sceneRenderTex;
	RenderTexture* downSampleTex;
	RenderTexture* horBlurTex;
	RenderTexture* verBlurTex;
	RenderTexture* upSampleTex;
	RenderTexture* glossTex;
	RenderTexture* bloomTex;
	OrthoMesh* downSampleMesh;
	OrthoMesh* upSampleMesh;
	TextureShader* texShader;
	BlurShaderVer* verBlurShader;
	BlurShaderHor* horBlurShader;
	GlossExtractShader* glossExtractShader;
	BloomShader* bloomShader;

	// post-processing togglers
	bool blurActive;
	bool invertColour;
	bool postProcessing;

	void renderToTexture();
	void downSampleTexture();
	void extractGloss();
	void mergeBloom();
	void horizontalBlur();
	void verticalBlur();
	void upSampleTexture();
	void renderToScreen();
	void renderScene();

	void renderShadowMap();
	RenderTexture* shadowDepthTexture;

	RenderTexture* cameraDepthTex;

	const int shadowMapResolution = 2048;
	DepthShader* depthShader;
	OrthoMesh* depthOrtho;
	XMMATRIX viewMatrix;
	XMMATRIX lightOrthoMatrix;

	void randomiseSpritePositions();
	void randomiseGrassPositions();
	XMFLOAT4 spritePositions[8];
	XMFLOAT4 grassPositions[8];
	int spritePositionRange = 10;
	const float spriteRepositionTime = 5.0f;
	float spriteTimer;
	const float spriteSpeed = 0.2f;

	float deltaTime;

	// UI dependancies
	float displacementFactor;
	float specularity;
	float reflectionAmnt;
	bool useSpecularMap;
	bool useWireframe;
	bool fog;
	float maxTessFactor = 3.0f;
	int grassSpriteNr = 4;
	int particleSpriteNr = 4;
	float bloomThreshold = 0.4f;
	bool useBloom = false;
	float maxFogAmnt = 0.5;
};

#endif