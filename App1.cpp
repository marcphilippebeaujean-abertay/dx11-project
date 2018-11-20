// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	//BaseApplication::BaseApplication();
	mesh = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	// Create Mesh object
	mesh = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext());
	skyMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	reflectionMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	cubeMesh = new TessellationCube(renderer->getDevice(), renderer->getDeviceContext());
	sphereMesh = new TessellationSphere(renderer->getDevice(), renderer->getDeviceContext());
	pointMesh = new PointMesh(renderer->getDevice(), renderer->getDeviceContext());

	teaCup = new Model(renderer->getDevice(), renderer->getDeviceContext(), "../res/teapot.obj");

	// add tessellation meshes to the vector
	tessMeshes.push_back(mesh);
	tessMeshes.push_back(cubeMesh);
	tessMeshes.push_back(sphereMesh);

	// add translation matrix for each tessellation mesh
	translationMatrices.push_back(renderer->getWorldMatrix()); // plane offset
	translationMatrices.push_back(XMMatrixTranslation(40, 15, 35)); // cube offsets
	translationMatrices.push_back(XMMatrixTranslation(30, 5, 25)); // sphere offset

	// load shaders
	tessellationShader = new TessellationShader(renderer->getDevice(), hwnd);
	tessellationBlendingShader = new TessellationBlendingShader(renderer->getDevice(), hwnd);
	colourShader = new ColourShader(renderer->getDevice(), hwnd);
	skyboxShader = new Skybox(renderer->getDevice(), hwnd);
	environmentMapShader = new EnvironmentMapShader(renderer->getDevice(), hwnd);
	verBlurShader = new BlurShaderVer(renderer->getDevice(), hwnd);
	horBlurShader = new BlurShaderHor(renderer->getDevice(), hwnd);
	texShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	billboardingShader = new GeometryShader(renderer->getDevice(), hwnd);
	glossExtractShader = new GlossExtractShader(renderer->getDevice(), hwnd);
	bloomShader = new BloomShader(renderer->getDevice(), hwnd);

	// load render textures
	sceneRenderTex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	downSampleTex = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	horBlurTex = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	verBlurTex = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	glossTex = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	bloomTex = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	upSampleTex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	shadowDepthTexture = new RenderTexture(renderer->getDevice(), shadowMapResolution, shadowMapResolution, SCREEN_NEAR, SCREEN_DEPTH);

	cameraDepthTex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// load orthographic meshes
	downSampleMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);
	upSampleMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);
	depthOrtho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);

	// load textures
	textureMgr->loadTexture("brick", L"../res/brickconcrete/Brick_ConcreteRetainingWall_512_d.png");
	textureMgr->loadTexture("brick_disp", L"../res/brickconcrete/Brick_ConcreteRetainingWall_512_h.png");
	textureMgr->loadTexture("brick_spec", L"../res/brickconcrete/Brick_ConcreteRetainingWall_512_s.png");
	textureMgr->loadTexture("cube_map", L"../res/skybox.dds");
	textureMgr->loadTexture("fire_sprite", L"../res/sprite.png");
	textureMgr->loadTexture("grass_sprite", L"../res/grasswip1.png");
	textureMgr->loadTexture("alpha", L"../res/extremealpha.png");
	textureMgr->loadTexture("bunny", L"../res/bunny.png");

	// initialise world matrices for scene objects
	XMMATRIX testOffset = XMMatrixTranslation(15, 4, 25);

	// setup scene objects
	testSceneObject = new SceneObject(textureMgr->getTexture("brick"), textureMgr->getTexture("default"), textureMgr->getTexture("brick_disp"), textureMgr->getTexture("brick_spec"), sphereMesh, testOffset);

	// setup lights
	MyLight* light1 = new MyLight;
	light1->setDiffuseColour(0.6f, 0.6f, 0.6f, 1.0f);
	light1->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	light1->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	light1->setSpecularPower(32.0f);
	light1->setRadius(30.0f);
	light1->setRange(30.0f);
	light1->setDirection(0, -0.8, 1.0);
	light1->setPosition(30, 20, 0);
	light1->setLookAt((light1->getPosition().x + light1->getDirection().x), (light1->getPosition().y + light1->getDirection().y), (light1->getPosition().z + light1->getDirection().z));
	light1->setSpotInnerAngle(0.75f);
	light1->setSpotOutterAngle(0.25f);
	light1->setLightType(0);
	// generate light matrices
	
	light1->generateViewMatrix();
	light1->generateOrthoMatrix(300, 300, SCREEN_NEAR, SCREEN_DEPTH);

	lights.push_back(light1);

	MyLight* light2 = new MyLight;
	light2->setDiffuseColour(0.0f, 1.0f, 0.0f, 1.0f);
	light2->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	light2->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	light2->setSpecularPower(32.0f);
	light2->setPosition(30, 10, 30);
	light2->setRadius(25.0f);
	light2->setRange(30.0f);
	light2->setDirection(0.5, 0, 1);
	light2->setSpotInnerAngle(1.0f);
	light2->setSpotOutterAngle(0.0f);
	light2->setLightType(2);

	lights.push_back(light2);

	//8, 0, 35

	MyLight* light3 = new MyLight;
	light3->setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	light3->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	light3->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	light3->setSpecularPower(32.0f);
	light3->setPosition(10, 10, 34);
	light3->setRadius(25.0f);
	light3->setRange(30.0f);
	light3->setDirection(0.5, 0, 1);
	light3->setSpotInnerAngle(1.0f);
	light3->setSpotOutterAngle(0.0f);
	light3->setLightType(1);

	lights.push_back(light3);

	// initialise post processing variables
	blurActive = false;
	invertColour = false;
	postProcessing = false;

	// setup delta time
	deltaTime = 0.0f;

	// create random seed
	srand(time(NULL));

	// populate sprite position array
	randomiseSpritePositions();
	randomiseGrassPositions();

	// initialise the timer
	spriteTimer = 0.0f;

	// initalise tessellation factor
	displacementFactor = 0.0f;

	// initialise specularity
	specularity = 3.0f;

	// specular map toggle
	useSpecularMap = true;

	fog = false;
	useWireframe = false;
	reflectionAmnt = 0.5f;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();


	if (billboardingShader)
	{
		delete billboardingShader;
		billboardingShader = nullptr;
	}
	// Release the Direct3D object.

	for (auto i = 0; i < tessMeshes.size(); i++)
	{
		delete tessMeshes[i];
		tessMeshes[i] = nullptr;
	}

	if (reflectionMesh)
	{
		delete reflectionMesh;
		reflectionMesh = nullptr;
	}

	if (pointMesh)
	{
		delete pointMesh;
		pointMesh = nullptr;
	}

	if (skyMesh)
	{
		delete skyMesh;
		skyMesh = nullptr;
	}

	if (colourShader)
	{
		delete colourShader;
		colourShader = nullptr;
	}

	if (tessellationShader)
	{
		delete tessellationShader;
		tessellationShader = nullptr;
	}

	if (environmentMapShader)
	{
		delete environmentMapShader;
		environmentMapShader = nullptr;
	}

	if(bloomShader)
	{
		delete bloomShader;
		bloomShader = nullptr;
	}

	if(bloomTex)
	{
		delete bloomTex;
		bloomTex = nullptr;
	}

	if (skyboxShader)
	{
		delete skyboxShader;
		skyboxShader = nullptr;
	}

	if(glossExtractShader)
	{
		delete glossExtractShader;
		glossExtractShader = nullptr;
	}

	for (auto i = 0; i < lights.size(); i++)
	{
		delete lights[i];
		lights[i] = nullptr;
	}

	if (blurRenderTarget)
	{
		delete blurRenderTarget;
		blurRenderTarget = nullptr;
	}

	if (sceneRenderTex)
	{
		delete sceneRenderTex;
		sceneRenderTex = nullptr;
	}

	if (downSampleTex)
	{
		delete downSampleTex;
		downSampleTex = nullptr;
	}

	if (horBlurTex)
	{
		delete horBlurTex;
		horBlurTex = nullptr;
	}

	if (verBlurTex)
	{
		delete verBlurTex;
		verBlurTex = nullptr;
	}

	if(glossTex)
	{
		delete glossTex;
		glossTex = nullptr;
	}

	if (upSampleTex)
	{
		delete upSampleTex;
		upSampleTex = nullptr;
	}

	if (downSampleMesh)
	{
		delete downSampleMesh;
		downSampleMesh = nullptr;
	}

	if (upSampleMesh)
	{
		delete upSampleMesh;
		upSampleMesh = nullptr;
	}

	if (texShader)
	{
		delete texShader;
		texShader = nullptr;
	}

	if (verBlurShader)
	{
		delete verBlurShader;
		verBlurShader = nullptr;
	}

	if (horBlurShader)
	{
		delete horBlurShader;
		horBlurShader = nullptr;
	}

	if (shadowDepthTexture)
	{
		delete shadowDepthTexture;
		shadowDepthTexture = nullptr;
	}

	if (cameraDepthTex)
	{
		delete cameraDepthTex;
		cameraDepthTex = nullptr;
	}

	if (depthShader)
	{
		delete depthShader;
		depthShader = nullptr;
	}

	if (billboardingShader)
	{
		delete billboardingShader;
		billboardingShader = nullptr;
	}

	if (depthOrtho)
	{
		delete depthOrtho;
		depthOrtho = nullptr;
	}

	if(teaCup)
	{
		delete teaCup;
		teaCup = nullptr;
	}

	if(tessellationBlendingShader)
	{
		delete tessellationBlendingShader;
		tessellationBlendingShader = nullptr;
	}
}

void App1::randomiseSpritePositions()
{
	// populate the position array
	for (auto i = 0; i < 8; i++)
	{
		// create random positions
		spritePositions[i] = XMFLOAT4(rand() % spritePositionRange + 0, rand() % spritePositionRange + 0, rand() % spritePositionRange + 0, 1);
	}
}

void App1::randomiseGrassPositions()
{
	// populate the position array
	for (auto i = 0; i < 8; i++)
	{
		// create random positions
		grassPositions[i] = XMFLOAT4(rand() % spritePositionRange + 0, 0.8, rand() % spritePositionRange + 0, 1);
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	// update the delta time
	deltaTime += timer->getTime();
	spriteTimer += timer->getTime();
	rotationZ = deltaTime;
	// check if our sprite timer has elapsed the time set before we change the sprite positions
	if (spriteTimer > spriteRepositionTime)
	{
		// reset timer
		spriteTimer = 0.0f;
		randomiseSpritePositions();
	}
	for (auto i = 0; i < 8; i++)
	{
		// increment the y position of the sprites over time
		spritePositions[i] = XMFLOAT4(spritePositions[i].x, (spritePositions[i].y + (spriteSpeed * timer->getTime())), spritePositions[i].z, spritePositions[i].w);
	}

	for(auto i = 0; i < lights.size(); i++)
	{
		lights[i]->setSpecularPower(specularity);
	}
	return true;
}

bool App1::render()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// render the shadow map
	renderShadowMap();

	if (postProcessing)
	{
		// render to the render texture (scene gets rendered here)
		renderToTexture();
		downSampleTexture();
		// gaussian blur
		if (blurActive)
		{
			// get the gloss map for bloom
			extractGloss();
			// apply horizontal blur
			horizontalBlur();
			// apply vertical blur
			verticalBlur();
			// apply the bloom effect by using the blurred gloss map and the downsampled render to textures
			mergeBloom();
		}
		// render the texture back to screen size
		upSampleTexture();
		renderer->setZBuffer(false);
		renderToScreen();
		renderer->setZBuffer(true);
	}
	else
	{
		renderScene();
	}

	// Render GUI
	gui();

	//// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::renderShadowMap()
{
	// Set the render target to be the depth texture.
	shadowDepthTexture->setRenderTarget(renderer->getDeviceContext());
	// Clear the render to texture.
	shadowDepthTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 0.0f, 1.0f);
	// render our tesellation objects
	for (auto i = 0; i < tessMeshes.size(); i++)
	{
		// displacement mesh only looks good on the plane, because the vertices are all facing the same direction
		float dispVal = 0.0f;
		if (i == 0)
		{
			dispVal = displacementFactor;
		}
		// send mesh information
		tessMeshes[i]->sendData(renderer->getDeviceContext());
		// Set shader parameters (matrices and texture)
		depthShader->setShaderParameters(renderer->getDeviceContext(), translationMatrices[i], lights[0]->getViewMatrix(), lights[0]->getOrthoMatrix(), dispVal, textureMgr->getTexture("brick_disp"), camera, XMFLOAT4(40.0f, 3.0f, maxTessFactor, 0.0f));
		// Render object (combination of mesh geometry and shader process)
		depthShader->render(renderer->getDeviceContext(), tessMeshes[i]->getIndexCount());
	}
	XMMATRIX teaPotMatrix = XMMatrixScaling(0.3, 0.3, 0.3) * XMMatrixRotationY(rotationZ) * XMMatrixTranslation(50, 3, 60);
	teaCup->sendData(renderer->getDeviceContext());
	// Set shader parameters (matrices and texture)
	depthShader->setShaderParameters(renderer->getDeviceContext(), teaPotMatrix, lights[0]->getViewMatrix(), lights[0]->getOrthoMatrix(), 0, textureMgr->getTexture("brick_disp"), camera, XMFLOAT4(40.0f, 3.0f, maxTessFactor, 0.0f));
	// Render object (combination of mesh geometry and shader process)
	depthShader->render(renderer->getDeviceContext(), teaCup->getIndexCount());
	//set render target back to the back buffer
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::renderScene()
{
	// Define matrices
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, skyMatrix, sphereOffset, bushOffset;

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	skyMatrix = worldMatrix;
	skyMatrix = XMMatrixTranslation(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);

	// Send geometry data for the sky mesh
	skyMesh->sendData(renderer->getDeviceContext());

	// turn off ZBuffer to render all other geometry on top of the skymesh
	renderer->setZBuffer(false);
	// disable culling for the skybox
	skyboxShader->toggleCulling(renderer->getDeviceContext(), false);
	//// render the skybox
	skyboxShader->setShaderParameters(renderer->getDeviceContext(), skyMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("cube_map"));
	skyboxShader->render(renderer->getDeviceContext(), skyMesh->getIndexCount());
	// enable it to render the rest of the scene
	skyboxShader->toggleCulling(renderer->getDeviceContext(), true);
	renderer->setZBuffer(true);

	renderer->setWireframeMode(useWireframe);

	// render our tesellation objects
	for (auto i = 0; i < tessMeshes.size(); i++)
	{
		// send mesh information
		tessMeshes[i]->sendData(renderer->getDeviceContext());

		// displacement mesh only looks good on the plane, because the vertices are all facing the same direction
		float dispVal = 0.0f;
		if (i == 0)
		{
			dispVal = displacementFactor;
		}
		// Set shader parameters (matrices and texture)
		tessellationShader->setShaderParameters(renderer->getDeviceContext(), translationMatrices[i], viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), dispVal, textureMgr->getTexture("brick_disp"), camera, useSpecularMap, textureMgr->getTexture("brick_spec"), lights, shadowDepthTexture->getShaderResourceView(), XMFLOAT4(40.0f, 3.0f, maxTessFactor, 0.0f));
		// Setup fog parameters
		tessellationShader->setFogParameters(renderer->getDeviceContext(), fog, 5.0f, 30.0f, maxFogAmnt, XMFLOAT3(0.5, 0.5, 0.5));
		// Render object (combination of mesh geometry and shader process
		tessellationShader->render(renderer->getDeviceContext(), tessMeshes[i]->getIndexCount());
	}

	teaCup->sendData(renderer->getDeviceContext());

	billboardingShader->toggleBlendState(renderer->getDeviceContext(), true);
	XMMATRIX teaPotMatrix = XMMatrixScaling(0.3, 0.3, 0.3) * XMMatrixRotationY(rotationZ) * XMMatrixTranslation(50, 3, 60);

	tessellationBlendingShader->setShaderParameters(renderer->getDeviceContext(), teaPotMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), 0.0, textureMgr->getTexture("brick_disp"), camera, useSpecularMap, textureMgr->getTexture("brick_spec"), lights, shadowDepthTexture->getShaderResourceView(), XMFLOAT4(40.0f, 3.0f, maxTessFactor, 0.0f), textureMgr->getTexture("alpha"), textureMgr->getTexture("bunny"));
	tessellationBlendingShader->render(renderer->getDeviceContext(), teaCup->getIndexCount());

	billboardingShader->toggleBlendState(renderer->getDeviceContext(), false);

	renderer->setWireframeMode(false);

	// setup reflection sphere offset
	sphereOffset = XMMatrixTranslation(0, 5, 0);

	// render sphere for reflection
	reflectionMesh->sendData(renderer->getDeviceContext());

	environmentMapShader->setShaderParameters(renderer->getDeviceContext(), sphereOffset, viewMatrix, projectionMatrix, textureMgr->getTexture("default"), textureMgr->getTexture("cube_map"), camera, lights[0]->getAmbientColour(), XMFLOAT4(1.0, 1.0, 1.0, 1.0), reflectionAmnt);
	environmentMapShader->render(renderer->getDeviceContext(), reflectionMesh->getIndexCount());

	bushOffset = XMMatrixTranslation(33, -.5, 20);
	XMMATRIX flyOffset = XMMatrixTranslation(8, 0, 35);

	billboardingShader->toggleBlendState(renderer->getDeviceContext(), true);
	// render the particle
	billboardingShader->setShaderParameters(renderer->getDeviceContext(), flyOffset, viewMatrix, projectionMatrix, textureMgr->getTexture("fire_sprite"), camera, XMFLOAT2(1.0f, 1.0f), spritePositions, false, static_cast<float>(particleSpriteNr));
	billboardingShader->render(renderer->getDeviceContext(), pointMesh->getIndexCount());

	billboardingShader->setShaderParameters(renderer->getDeviceContext(), bushOffset, viewMatrix, projectionMatrix, textureMgr->getTexture("grass_sprite"), camera, XMFLOAT2(1.0f, 1.0f), grassPositions, true, static_cast<float>(grassSpriteNr));
	billboardingShader->render(renderer->getDeviceContext(), pointMesh->getIndexCount());
	billboardingShader->toggleBlendState(renderer->getDeviceContext(), false);
}

void App1::renderToTexture()
{
	// Set the render target to be the render to texture.
	sceneRenderTex->setRenderTarget(renderer->getDeviceContext());
	// Clear the render to texture.
	sceneRenderTex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
	// render the scene as normal - it will be sent to the render target, which is now or ortho mesh
	renderScene();
}

void App1::downSampleTexture()
{
	// Set the render target to be the render to texture.
	downSampleTex->setRenderTarget(renderer->getDeviceContext());
	downSampleTex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
	// setup matrices to project scene on to the "downsampled" ortho mesh
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	// use the texture shader to render the scene onto our down sampled mesh
	downSampleMesh->sendData(renderer->getDeviceContext());
	texShader->setShaderParameters(renderer->getDeviceContext(), renderer->getWorldMatrix(), orthoViewMatrix, orthoMatrix, sceneRenderTex->getShaderResourceView(), false);
	texShader->render(renderer->getDeviceContext(), downSampleMesh->getIndexCount());
}

void App1::extractGloss()
{
	// Set the render texture
	glossTex->setRenderTarget(renderer->getDeviceContext());
	glossTex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
	// setup matrices to project scene on to the "downsampled" ortho mesh
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	downSampleMesh->sendData(renderer->getDeviceContext());
	glossExtractShader->setShaderParameters(renderer->getDeviceContext(), renderer->getWorldMatrix(), orthoViewMatrix, orthoMatrix, downSampleTex->getShaderResourceView(), bloomThreshold);
	glossExtractShader->render(renderer->getDeviceContext(), downSampleMesh->getIndexCount());
}


void App1::horizontalBlur()
{
	// Set the render target to be the render to texture.
	horBlurTex->setRenderTarget(renderer->getDeviceContext());
	horBlurTex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
	ID3D11ShaderResourceView* resourceView;
	if(useBloom)
	{
		resourceView = glossTex->getShaderResourceView();
	}
	else
	{
		resourceView = downSampleTex->getShaderResourceView();
	}
	// setup matrices to project scene on to the "downsampled" ortho mesh
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	// use the blur shader to render the scene onto our down sampled mesh
	downSampleMesh->sendData(renderer->getDeviceContext());
	horBlurShader->setShaderParameters(renderer->getDeviceContext(), renderer->getWorldMatrix(), orthoViewMatrix, orthoMatrix, resourceView, sWidth);
	horBlurShader->render(renderer->getDeviceContext(), downSampleMesh->getIndexCount());
}

void App1::verticalBlur()
{
	verBlurTex->setRenderTarget(renderer->getDeviceContext());
	verBlurTex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
	// no need to reset render target, because we are going to apply the vertical blur to the same texture as the horizontal one
	// setup matrices to project scene on to the "downsampled" ortho mesh
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	// apply vertical blur to the down sampled mesh, to complete the blur process
	downSampleMesh->sendData(renderer->getDeviceContext());
	verBlurShader->setShaderParameters(renderer->getDeviceContext(), renderer->getWorldMatrix(), orthoViewMatrix, orthoMatrix, horBlurTex->getShaderResourceView(), sHeight);
	verBlurShader->render(renderer->getDeviceContext(), downSampleMesh->getIndexCount());
}

void App1::mergeBloom()
{
	bloomTex->setRenderTarget(renderer->getDeviceContext());
	bloomTex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
	// setup matrices to project scene on to the "up samapled" ortho mesh
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	downSampleMesh->sendData(renderer->getDeviceContext());
	bloomShader->setShaderParameters(renderer->getDeviceContext(), renderer->getWorldMatrix(), orthoViewMatrix, orthoMatrix, verBlurTex->getShaderResourceView(), downSampleTex->getShaderResourceView());
	bloomShader->render(renderer->getDeviceContext(), downSampleMesh->getIndexCount());
}

void App1::upSampleTexture()
{
	// Set the render target to be the render to texture.
	upSampleTex->setRenderTarget(renderer->getDeviceContext());
	upSampleTex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
	// setup matrices to project scene on to the "up samapled" ortho mesh
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	// use the texture shader to render the blurred scene onto our up sampled mesh
	upSampleMesh->sendData(renderer->getDeviceContext());
	ID3D11ShaderResourceView* resourceView;
	if (blurActive)
	{
		if (useBloom)
		{
			resourceView = bloomTex->getShaderResourceView();
		}
		else
		{
			resourceView = verBlurTex->getShaderResourceView();
		}
	}
	else
	{
		resourceView = downSampleTex->getShaderResourceView();
	}
	texShader->setShaderParameters(renderer->getDeviceContext(), renderer->getWorldMatrix(), orthoViewMatrix, orthoMatrix, resourceView, false);
	texShader->render(renderer->getDeviceContext(), upSampleMesh->getIndexCount());
}

void App1::renderToScreen()
{
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
	// disable zbuffer when trying to project the ortho mesh onto the screen
	// setup matrices to project scene on to the ortho mesh
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	// use the texture shader to display the other camera on the mesh
	upSampleMesh->sendData(renderer->getDeviceContext());
	texShader->setShaderParameters(renderer->getDeviceContext(), renderer->getWorldMatrix(), orthoViewMatrix, orthoMatrix, upSampleTex->getShaderResourceView(), invertColour); // apply colour inversion here
	texShader->render(renderer->getDeviceContext(), upSampleMesh->getIndexCount());
}

void App1::gui()
{
	// Force turn off on Geometry shader
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe Mode", &useWireframe);
	ImGui::Checkbox("Post Processing", &postProcessing);
	ImGui::Checkbox("Blur", &blurActive);
	ImGui::Checkbox("Bloom", &useBloom);
	ImGui::SliderFloat("Bloom Threshold", &bloomThreshold, 0.0f, 1.0f);
	ImGui::Checkbox("Invert Colour", &invertColour);
	ImGui::SliderFloat("Displacement Factor", &displacementFactor, 0.0f, 2.0f);
	ImGui::SliderFloat("Specular Power", &specularity, 0.0f, 64.0f);
	ImGui::Checkbox("Use Specular Map", &useSpecularMap);
	ImGui::Checkbox("Fog", &fog);
	ImGui::SliderFloat("Fog Intensity", &maxFogAmnt, 0.0f, 1.0f);
	ImGui::SliderFloat("Reflection Amount", &reflectionAmnt, 0.0f, 1.0f);
	ImGui::SliderInt("Sprite Position Range", &spritePositionRange, 3, 10);
	ImGui::SliderFloat("Max Tessellation", &maxTessFactor, 1.0f, 6.0f);
	ImGui::SliderInt("Particle Sprites", &particleSpriteNr, 1, 8);
	ImGui::SliderInt("Bush Sprites", &grassSpriteNr, 1, 8);

	// Render UI
	ImGui::Render();
}

