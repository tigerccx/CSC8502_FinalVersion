#include "Renderer.h"

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {


	//
	//Init camera
	//
	auto cubeMap = make_shared<Texture>(GL_TEXTURE_CUBE_MAP);
	array<string, 6> cubeMapPaths1 = { TEXTUREDIR "Skybox/Deep Dusk__Cam_2_Left+X.png" , TEXTUREDIR "Skybox/Deep Dusk__Cam_3_Right-X.png",
									  TEXTUREDIR "Skybox/Deep Dusk__Cam_4_Up+Y.png" , TEXTUREDIR "Skybox/Deep Dusk__Cam_5_Down-Y.png",
									  TEXTUREDIR "Skybox/Deep Dusk__Cam_0_Front+Z.png", TEXTUREDIR "Skybox/Deep Dusk__Cam_1_Back-Z.png" };
	cubeMap->LoadCubeMap(cubeMapPaths1, 0);

	auto goCam = GameObjectManager::GetInstance()->CreateGameObject();
	goCam->name = "Camera";
	CameraParamPerspective camParam;
	camParam.nearPlane = 1.0f;
	camParam.farPlane = 10000.0f;
	camParam.fov = 45;
	camParam.aspectWH = float(width) / float(height);
	Camera* cam = CameraManager::GetInstance()->CreateCamera();
	cam->SetCameraParameters(camParam);
	cam->SetDepthTest(true);
	cam->SetClearFlag(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	cam->SetFaceCulling(true);
	cam->SetFaceCullingType(GL_BACK);
	cam->SetCameraRenderTarget(width, height);
	renderTargetCam = cam->GetCameraRenderTarget();
	cam->SetClearType(CameraClearType_Skybox);
	shared_ptr<Skybox> skybox = make_shared<Skybox>();
	skybox->SetCubeMap(cubeMap);
	cam->SetSkybox(skybox);
	goCam->AddComponentAndActivate(cam);
	goCam->AddComponentAndActivate(new CameraController(10));
	goCam->AddComponentAndActivate(new CameraAutoController());
	goCam->transform->SetPositionWorld(-100.0f, 5.0f, 10.0f);
	goCam->transform->SetRotationWorld(10.0f, -90.0f, 0.0f);


	//
	//Init Gizmo
	//
	auto shaderGizmo = ShaderManager::GetInstance()->CreateShader("VanillaVertex.glsl", "VanillaFragment.glsl");
	if (!shaderGizmo || !shaderGizmo->LoadSuccess()) {
		return;
	}
	auto matGizmo = MaterialManager::GetInstance()->CreateMaterial("GIZMO_Grid");
	matGizmo->renderMode = MaterialRenderMode_Common;
	matGizmo->SetShader(shaderGizmo);
	auto meshMaterialGizmo = make_shared<MeshMaterial>();
	meshMaterialGizmo->AddMaterial(matGizmo);

	/*goGrid = GameObjectManager::GetInstance()->CreateGameObject();
	goGrid->name = "GIZMO_Grid";
	auto meshRendererGrid = RendererManager::GetInstance()->CreateRenderer();
	meshRendererGrid->SetMesh(Geometry::GenerateGrid(1.0f, 100));
	meshRendererGrid->SetMeshMaterial(meshMaterialGizmo);
	goGrid->AddComponentAndActivate(meshRendererGrid);*/

	auto goXYZ = GameObjectManager::GetInstance()->CreateGameObject();
	goXYZ->name = "GIZMO_XYZ";
	auto meshRendererXYZ = RendererManager::GetInstance()->CreateRenderer();
	auto meshXYZ = Geometry::GenerateXYZ();
	meshRendererXYZ->SetMesh(meshXYZ);
	meshRendererXYZ->SetMeshMaterial(meshMaterialGizmo);
	goXYZ->AddComponentAndActivate(meshRendererXYZ);
	goXYZ->transform->SetScaleLocal(2, 2, 2);
	goXYZ->transform->SetPositionWorld(0, 0, 0);

	// Shaders
	auto sceneShader = ShaderManager::GetInstance()->CreateShader("PerPixel_Shadow_Bumpmap_Vertex.glsl", "PerPixel_Shadow_PBR_Fragment.glsl");
	if (!sceneShader || !sceneShader->LoadSuccess()) {
		return;
	}

	auto sceneShaderInstanced = ShaderManager::GetInstance()->CreateShader("PerPixel_Shadow_Bumpmap_Vertex_Instanced.glsl", "PerPixel_Shadow_PBR_Fragment.glsl");
	if (!sceneShaderInstanced || !sceneShaderInstanced->LoadSuccess()) {
		return;
	}
	
	auto transparentShader = ShaderManager::GetInstance()->CreateShader("TexturedVertex.glsl", "texturedAlphaFragment.glsl");
	if (!transparentShader || !transparentShader->LoadSuccess()) {
		return;
	}

	auto shaderMirror = ShaderManager::GetInstance()->CreateShader("MirrorVertex.glsl", "MirrorFragment.glsl");
	if (!shaderMirror || !shaderMirror->LoadSuccess()) {
		return;
	}

	auto shaderAllInOne = ShaderManager::GetInstance()->CreateShader("PerPixel_AllInOne_Vertex.glsl", "PerPixel_AllInOne_Fragment.glsl");
	if (!shaderAllInOne || !shaderAllInOne->LoadSuccess()) {
		return;
	}

	auto shaderTessTest = ShaderManager::GetInstance()->CreateShader("Water1_Vert.glsl", "Water1_PBR_Frag.glsl", "Water1_Geo.glsl", "Water1_TCS.glsl", "Water1_TES.glsl");
	if (!shaderTessTest || !shaderTessTest->LoadSuccess()) {
		return;
	}

	auto shaderTerrain = ShaderManager::GetInstance()->CreateShader("PerPixel_AllInOne_Vertex.glsl", "Terrain1_PBR_Frag.glsl");
	if (!shaderTerrain || !shaderTerrain->LoadSuccess()) {
		return;
	}

	//
	//Textures
	//
	auto textureTerrain = make_shared<Texture>();
	textureTerrain->Load(TEXTUREDIR "Terrains/HeightMapTex0.jpg", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureTerrain->SetTextureRepeat(true);
	
	auto textureWater = make_shared<Texture>();
	//textureWater->Load(TEXTUREDIR "Water/water.png", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureWater->Load(TEXTUREDIR "Water/water.tga", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureWater->SetTextureRepeat(true);

	auto textureWaterBump = make_shared<Texture>();
	textureWaterBump->Load(TEXTUREDIR "Water/waterbump.png", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureWaterBump->SetTextureRepeat(true);

	auto textureWaterOffset = make_shared<Texture>();
	textureWaterOffset->Load(TEXTUREDIR "Water/OffsetTest2.png", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureWaterOffset->SetTextureRepeat(true);

	auto textureSand = make_shared<Texture>();
	textureSand->Load(TEXTUREDIR "Terrains/sand_diffuse.tga", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureSand->SetTextureRepeat(true);
	auto textureSandBump = make_shared<Texture>();
	textureSandBump->Load(TEXTUREDIR "Terrains/sand_normal.tga", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureSandBump->SetTextureRepeat(true);

	auto textureGrass = make_shared<Texture>();
	textureGrass->Load(TEXTUREDIR "Terrains/grass_diffuse.png", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureGrass->SetTextureRepeat(true);
	auto textureGrassBump = make_shared<Texture>();
	textureGrassBump->Load(TEXTUREDIR "Terrains/grass_normal.png", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureGrassBump->SetTextureRepeat(true);

	auto textureStone = make_shared<Texture>();
	textureStone->Load(TEXTUREDIR "Terrains/stone_diffuse.tga", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureStone->SetTextureRepeat(true);
	auto textureStoneBump = make_shared<Texture>();
	textureStoneBump->Load(TEXTUREDIR "Terrains/stone_normal.tga", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureStoneBump->SetTextureRepeat(true);
	
	auto textureTransparent = make_shared<Texture>();
	textureTransparent->Load(TEXTUREDIR "stainedglass.tga", SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	textureTransparent->SetTextureRepeat(true);

	//
	//Meshes
	//
	auto meshQuadWithNormalTess = Geometry::GenerateQuad(true);
	meshQuadWithNormalTess->SetRenderMode(GL_PATCHES);
	auto meshQuadWithNormal = Geometry::GenerateQuad(true);

	auto meshImportedOakTree = make_shared<Mesh>();
	meshImportedOakTree->LoadFromMeshFile("Oak_Tree.msh");
	auto meshImportedPalmTree = make_shared<Mesh>();
	meshImportedPalmTree->LoadFromMeshFile("Palm_Tree.msh");

	auto meshPlane = Geometry::GeneratePlane();

	auto meshCube = Geometry::GenerateCube();


	//
	////
	//Init GOs
	////
	// 
	// 
	//
	////Cube (Child Object)
	////
	//array<GameObject*, 5> goCubes;
	//for (int i = 0; i < 5; ++i) {
	//	auto goCube = GameObjectManager::GetInstance()->CreateGameObject();
	//	goCube->name = "GIZMO_CUBE";
	//	auto meshRendererCube = RendererManager::GetInstance()->CreateRenderer();
	//	meshRendererCube->SetMesh(meshCube);
	//	meshRendererCube->SetMeshMaterial(meshMaterialGizmo);
	//	goCube->AddComponentAndActivate(meshRendererCube);
	//	goCubes[i] = goCube;
	//}
	

	//
	//Transparent
	//
	auto goQuad = GameObjectManager::GetInstance()->CreateGameObject();
	goQuad->name = "Transparent Thing";
	auto meshRendererTransparent = RendererManager::GetInstance()->CreateRenderer();
	auto meshMaterialTransparent = make_shared<MeshMaterial>();
	auto matTransparent = MaterialManager::GetInstance()->CreateMaterial("TestTransparent");
	matTransparent->SetShader(transparentShader);
	matTransparent->SetTexture("diffuseTex", textureTransparent);
	matTransparent->renderMode = MaterialRenderMode_Transparent;
	meshRendererTransparent->SetMesh(meshPlane);
	meshMaterialTransparent->AddMaterial(matTransparent);
	meshRendererTransparent->SetMeshMaterial(meshMaterialTransparent);
	goQuad->AddComponentAndActivate(meshRendererTransparent);
	goQuad->transform->SetPositionWorld(20, 10, 90);
	goQuad->transform->SetScaleLocal(5, 1, 5);

	auto goQuad1 = GameObjectManager::GetInstance()->CreateGameObject();
	goQuad1->name = "Transparent Thing1";
	auto meshRendererTransparent1 = RendererManager::GetInstance()->CreateRenderer();
	meshRendererTransparent1->SetMesh(meshPlane);
	meshRendererTransparent1->SetMeshMaterial(meshMaterialTransparent);
	goQuad1->AddComponentAndActivate(meshRendererTransparent1);
	goQuad1->transform->SetPositionWorld(20, 7, 90);
	goQuad1->transform->SetScaleLocal(5, 1, 5);

	//
	//Water
	//
	auto meshMaterialTess = make_shared<MeshMaterial>();
	auto matTess = MaterialManager::GetInstance()->CreateMaterial("TessTest");
	matTess->SetShader(shaderTessTest);
	matTess->SetTexture("offsetTex", textureWaterOffset);
	matTess->SetTexture("offsetTex1", textureWaterOffset);
	matTess->SetFloat("texScale0", 2);
	matTess->SetFloat("texScale1", 20);
	matTess->SetFloat("offsetScale0", 4);
	matTess->SetFloat("offsetScale1", 1);
	matTess->SetTexture("diffuseTex", textureWater);
	matTess->SetInt("useReflection", 1);
	matTess->SetFloat("reflectionCoeff", 0.8);
	matTess->SetTexture("cubeTex", cubeMap);
	matTess->SetTexture("bumpTex", textureWaterBump);
	matTess->SetFloat("ambientComponent", 0.03);
	matTess->SetFloat("metallic", 0.05);
	matTess->SetFloat("smoothness", 0.9);
	matTess->Set3FloatVector("diffuseComponent", Vector3(0.5, 0.5, 0.5));
	matTess->Set3FloatVector("specularComponent", Vector3(0.5, 0.5, 0.5));
	matTess->renderMode = MaterialRenderMode_Transparent;

	meshMaterialTess->AddMaterial(matTess);
	auto meshRendererTess = RendererManager::GetInstance()->CreateRenderer();
	meshRendererTess->SetMesh(meshQuadWithNormalTess);
	meshRendererTess->SetMeshMaterial(meshMaterialTess);
	meshRendererTess->alwaysRender = false;
	auto goTess = GameObjectManager::GetInstance()->CreateGameObject();
	goTess->name = "Tess Test";
	goTess->AddComponentAndActivate(meshRendererTess);
	goTess->transform->SetPositionWorld(0, 2.5, 0);
	goTess->transform->SetScaleLocal(200, 200, 1);
	goTess->transform->SetRotationLocal(-90, 0, 0);
	auto testAnimator0 = new WaterAnimator();
	testAnimator0->cam = cam;
	testAnimator0->mat = matTess;
	goTess->AddComponentAndActivate(testAnimator0);

	//
	// Terrain
	// 
	//Load heightmap from textureTest0
	auto heightMap = make_shared<HeightMap>((TEXTUREDIR"Terrains/HeightMap5.png"));
	if (!heightMap->IsInit()) {
		cout << "Height Map init error!" << endl;
	}
	Vector3 dimensions = heightMap->GetHeightmapSize();

	auto matTerrain = MaterialManager::GetInstance()->CreateMaterial("IHavetheHighGround");
	matTerrain->renderMode = MaterialRenderMode_Common;
	matTerrain->SetShader(shaderTerrain);

	matTerrain->SetInt("bumpTex_use", 1);
	matTerrain->SetTexture("diffuseTex0", textureSand);
	matTerrain->SetTexture("bumpTex0", textureSandBump);
	matTerrain->SetFloat("metallic0", 0.0);
	matTerrain->SetFloat("smoothness0", 0.02);
	matTerrain->SetTexture("diffuseTex1", textureGrass);
	matTerrain->SetTexture("bumpTex1", textureGrassBump);
	matTerrain->SetFloat("metallic1", 0.0);
	matTerrain->SetFloat("smoothness1", 0.05);
	matTerrain->SetTexture("diffuseTex2", textureStone);
	matTerrain->SetTexture("bumpTex2", textureStoneBump);
	matTerrain->SetFloat("metallic2", 0.05);
	matTerrain->SetFloat("smoothness2", 0.2);

	matTerrain->SetFloat("thres00", 1 * 4);
	matTerrain->SetFloat("thres01", 2 * 4);
	matTerrain->SetFloat("thres10", 3 * 4);
	matTerrain->SetFloat("thres11", 4 * 4);

	matTess->SetFloat("ambientComponent", 0.03);
	matTerrain->SetInt("useReflection", 0);
	auto meshRendererTerrain = RendererManager::GetInstance()->CreateRenderer();
	auto meshMaterialTerrain = make_shared<MeshMaterial>();
	meshMaterialTerrain->AddMaterial(matTerrain);
	meshRendererTerrain->SetMeshMaterial(meshMaterialTerrain);
	meshRendererTerrain->SetMesh(heightMap);
	meshRendererTerrain->castShadow = true;

	auto goMap = GameObjectManager::GetInstance()->CreateGameObject();
	goMap->name = "HeightMap";
	goMap->AddComponentAndActivate(meshRendererTerrain);
	goMap->transform->SetScaleLocal(Vector3(0.005, 0.03, 0.005)*4);
	goMap->transform->SetPositionWorld(Vector3(-dimensions.x * 0.0025, 0, -dimensions.z * 0.0025)*4);


	//
	//Object Import
	//

	Vector3* heightMapVertices = heightMap->GetVertices();
	int countVertices = heightMap->GetNumVertices();
	Matrix4 matWorldHeightMap = goMap->transform->GetModelMatrix();

	//Oak Tree
	auto meshMaterialTree = make_shared<MeshMaterial>("Oak_Tree.mat", false, false);
	auto matTree = meshMaterialTree->GetMaterial("Free_Tree");
	matTree->SetShader(sceneShaderInstanced);
	matTree->SetInt("useBumpTex", 0);
	matTree->SetFloat("ambientComponent", 0.03);
	matTree->SetFloat("metallic", 0.0);
	matTree->SetFloat("smoothness", 0.1);
	matTree->SetFloat("specularGloss", 60.0);


	float chanceTree = 0.005f;
	float chancePalmTree = 0.7f;
	float thresGenerateTreeMinHeight = 4.5;
	float thresGenerateTreeMaxHeight = 15;

	int countTree = 0;
	for (int i = 0; i < countVertices; ++i) {
		//Generate Tree if ...
		Vector3 posVertex = (matWorldHeightMap * Vector4(heightMapVertices[i], 1)).ToVector3();

		if (posVertex.y> thresGenerateTreeMinHeight && posVertex.y < thresGenerateTreeMaxHeight &&  MyMaths::RandomFloat01() < chanceTree) {
			auto goImport = GameObjectManager::GetInstance()->CreateGameObject();
			goImport->name = "Tree_" + to_string(countTree);
			auto meshRendererImport = RendererManager::GetInstance()->CreateRenderer();
			if (MyMaths::RandomFloat01() < chancePalmTree) {
				meshRendererImport->SetMesh(meshImportedPalmTree);
			}
			else
			{
				meshRendererImport->SetMesh(meshImportedOakTree);
			}
			
			meshRendererImport->SetMeshMaterial(meshMaterialTree);
			meshRendererImport->castShadow = true;
			meshRendererImport->enableInstancing = true;
			goImport->AddComponentAndActivate(meshRendererImport);
			goImport->transform->SetPositionWorld(posVertex-Vector3(0,0.1,0));
			goImport->transform->SetRotationWorld(MyMaths::RandomFloatN1P1()*5, MyMaths::RandomFloatN1P1() * 360, MyMaths::RandomFloatN1P1() * 5);
			float scale = 1 - MyMaths::RandomFloat01() * 0.3;
			goImport->transform->SetScaleLocal(scale, scale, scale);

			++countTree;
		}
	}

	//
	//Mirror
	//
	auto goMirror = GameObjectManager::GetInstance()->CreateGameObject();
	auto goCam2 = GameObjectManager::GetInstance()->CreateGameObject();
	goCam2->name = "CameraMirror";
	Camera* cam2 = CameraManager::GetInstance()->CreateCamera();
	cam2->SetCameraParameters(camParam);
	cam2->SetClearType(CameraClearType_Skybox);
	cam2->SetClearFlag(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	cam2->SetFaceCulling(true);
	cam2->SetFaceCullingType(GL_BACK);
	cam2->SetSkybox(skybox);
	cam2->SetRenderOrder(-2);
	cam2->SetCameraRenderTarget(width, height);
	goCam2->AddComponentAndActivate(cam2);
	auto mirrorController = new MirrorCameraController();
	mirrorController->transRefCam = goCam->transform;
	mirrorController->transMirrorCam = goCam2->transform;
	mirrorController->mirrorCam = cam2;
	goMirror->AddComponentAndActivate(mirrorController);
	auto meshMatMirror = make_shared<MeshMaterial>();
	auto matMirror = MaterialManager::GetInstance()->CreateMaterial("Mirror");
	matMirror->SetShader(shaderMirror);
	matMirror->SetTexture("diffuseTex", cam2->GetCameraRenderTarget()->GetColourTexture());
	mirrorController->matMirror = matMirror;
	meshMatMirror->AddMaterial(matMirror);
	auto meshRendererMirror = RendererManager::GetInstance()->CreateRenderer();
	meshRendererMirror->SetMesh(meshPlane);
	meshRendererMirror->SetMeshMaterial(meshMatMirror);
	mirrorController->rendererSelf = meshRendererMirror;
	goMirror->AddComponentAndActivate(meshRendererMirror);
	goMirror->transform->SetRotationWorld(90, 0, 0);
	Vector3 a = goMirror->transform->GetRotationWorld().ToEuler();
	goMirror->transform->SetScaleLocal(100, 1, 100);
	goMirror->transform->SetPositionWorld(0, 0, -100);


	//
	//Create lights
	//
	
	array<GameObject*, 5> goLights;
	array<Light*, 5> lights;
	for (int i = 0; i < 5; ++i) {
		auto goLight = GameObjectManager::GetInstance()->CreateGameObject();
		goLight->name = "Light" + to_string(i);
		lights[i] = LightManager::GetInstance()->CreateLight();
		lights[i]->SetLightType(LightType_Point);
		goLight->AddComponentAndActivate(lights[i]);
		auto mr = RendererManager::GetInstance()->CreateRenderer();
		mr->SetMesh(meshXYZ);
		mr->SetMeshMaterial(meshMaterialGizmo);
		goLight->AddComponentAndActivate(mr);
		goLights[i] = goLight;
		//goCubes[i]->transform->SetParent(goLights[i]->transform);
	}

	PointLightParameters plp;
	SpotLightParameters slp;

	goLights[0]->transform->SetPositionWorld(-20.0f, 10.0f, -20.0f);
	goLights[0]->transform->Rotate(-15.0f, 0.0, 0.0f);
	lights[0]->SetLightColourAndIntensity(Vector4(1, 0.4, 0.2, 1.4));
	lights[0]->SetLightType(LightType_Directional);
	lights[0]->SetCastShadow(true);
	lights[0]->SetShadowMapSize(4096);

	goLights[1]->transform->SetPositionWorld(30.0f, 16.0f, -30.0f);
	goLights[1]->transform->Rotate(-10.0f, 45.0f, 0.0f);
	lights[1]->SetLightColourAndIntensity(Vector4(1, 1, 1, 380));
	slp.radius = 100.0f;
	slp.angle = 45.0f;
	lights[1]->SetLightType(LightType_Spot);
	lights[1]->SetSpotLightParameters(slp);
	lights[1]->SetCastShadow(true);
	lights[1]->SetShadowMapSize(512);

	goLights[2]->transform->SetPositionWorld(-60.0f, 15.0f, -25.0f);
	goLights[2]->transform->Rotate(0, 0, 0);
	lights[2]->SetLightColourAndIntensity(Vector4(1, 1, 1, 20));
	plp.radius = 200.0f;
	lights[2]->SetLightType(LightType_Point);
	lights[2]->SetPointLightParameters(plp);
	lights[2]->SetCastShadow(true);
	lights[2]->SetShadowMapSize(512);

	goLights[3]->transform->SetPositionWorld(50.0f, 20.0f, 0.0f);
	goLights[3]->transform->Rotate(0, 0, 0);
	lights[3]->SetLightColourAndIntensity(Vector4(1, 0.7, 0.8, 300));
	plp.radius = 150.0f;
	lights[3]->SetLightType(LightType_Point);
	lights[3]->SetPointLightParameters(plp);
	lights[3]->SetCastShadow(true);
	lights[3]->SetShadowMapSize(512);

	goLights[4]->transform->SetPositionWorld(25.0f, 21.0f, -5.0f);
	goLights[4]->transform->Rotate(0, 0, 0);
	lights[4]->SetLightColourAndIntensity(Vector4(0, 1, 0.6, 250));
	plp.radius = 100.0f;
	lights[4]->SetLightType(LightType_Point);
	lights[4]->SetPointLightParameters(plp);
	lights[4]->SetCastShadow(true);
	lights[4]->SetShadowMapSize(1024);

	//
	////
	//PostProcessing Setup
	////
	//
	ppoVolumetricLighting.SetupPostProcessObject(width, height);
	ppoVolumetricLighting.SetRenderScale(0.25f);
	ppoVolumetricLighting.camRender = cam;

	ppoGaussianBlur.SetCountPass(10);
	ppoGaussianBlur.SetupPostProcessObject(width, height);

	ppoSobel.SetCountPass(1);
	ppoSobel.SetupPostProcessObject(width, height);

	ppoDoubleVision.SetDoubleVisionOffset(Vector2::zero);
	ppoDoubleVision.SetupPostProcessObject(width, height);
	auto goDoubleVisionController = GameObjectManager::GetInstance()->CreateGameObject();
	goDoubleVisionController->AddComponentAndActivate(new DoubleVisionController(&ppoDoubleVision, 60.0f));

	ppoColourGrading.SetStages(10);
	ppoColourGrading.SetupPostProcessObject(width, height);

	ppoToScreen.SetupPostProcessObject(width, height);
	
	ppoList.emplace_back(&ppoVolumetricLighting);
	ppoVolumetricLighting.isEnabled = false;
	ppoList.emplace_back(&ppoGaussianBlur);
	ppoGaussianBlur.isEnabled = false;
	ppoList.emplace_back(&ppoDoubleVision);
	ppoDoubleVision.isEnabled = false;
	ppoList.emplace_back(&ppoColourGrading);
	ppoColourGrading.isEnabled = false;

	//
	//GamePlay Controller
	//
	auto goGame = GameObjectManager::GetInstance()->CreateGameObject();
	GameController* gc = new GameController();
	gc->transInControl = goLights[0]->transform;
	gc->mainCam = cam;
	gc->cubemapNight = cubeMap;
	gc->matWater = matTess;
	gc->goDirectionalLight = goLights[0];
	gc->lightDirection = lights[0];
	gc->goSpotlight = goLights[1];
	gc->lightSpot = lights[1];
	gc->goMirror = goMirror;
	gc->goPointlights.emplace_back(goLights[2]);
	gc->goPointlights.emplace_back(goLights[3]);
	gc->goPointlights.emplace_back(goLights[4]);
	gc->lightsPoint.emplace_back(lights[2]);
	gc->lightsPoint.emplace_back(lights[3]);
	gc->lightsPoint.emplace_back(lights[4]);

	gc->ppoVolumetricLighting = &ppoVolumetricLighting;
	gc->ppoGaussian = &ppoGaussianBlur;
	gc->ppoDoubleVision = &ppoDoubleVision;
	gc->ppoColourGrading = &ppoColourGrading;


	goGame->AddComponentAndActivate(gc);




	//
	////
	//Other Setups
	////
	//



	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Draw frame
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Enable seamless cube map sampling
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer() {
	
}

void Renderer::UpdateScene(float dt) {
	GameObjectManager::GetInstance()->Update(dt);
}

void Renderer::RenderScene() {
	GameObjectManager::GetInstance()->PreRender();
	DrawShadowScene();
	DrawScene();
	DrawPostProcess();
	PresentScene();
	GameObjectManager::GetInstance()->PostRender();
}

void Renderer::DrawScene()
{
	RendererManager::GetInstance()->RenderAllCameras();
}

void Renderer::DrawPostProcess()
{
	renderTargetCur = renderTargetCam;
	for (int i = 0; i < ppoList.size(); ++i) {
		if (ppoList[i]->isEnabled) {
			ppoList[i]->RenderPostProcess(renderTargetCur);
			renderTargetCur = ppoList[i]->GetFinalResult();
		}
	}
}

void Renderer::PresentScene()
{
	ppoToScreen.RenderPostProcess(renderTargetCur);
}

void Renderer::DrawShadowScene()
{
	RendererManager::GetInstance()->RenderShadowMaps();
}
