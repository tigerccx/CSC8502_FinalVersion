#pragma once

#include <array>

#include "OGLRenderer.h"
#include "MeshRenderer.h"
#include "HeightMap.h"
#include "Camera.h"
#include "CameraController.h"
#include "Geometry.h"
#include "Light.h"
#include "RenderTargetTexture.h"

#include "ScreenQuad.h"

#include "WaterAnimator.h"
#include "MirrorCameraController.h"
#include "CameraAutoController.h"'
#include "GameController.h"


#include "PPO_GaussianBlur.h"
#include "PPO_ToScreen.h"
#include "PPO_Sobel.h"
#include "PPO_DoubleVision.h"
#include "DoubleVisionController.h"
#include "PPO_ColourGrading.h"
#include "PPO_VolumetricLighting.h"


#include "CameraManager.h"
#include "GameObjectManager.h"
#include "MaterialManager.h"
#include "ShaderManager.h"
#include "RendererManager.h"
#include "LightManager.h"




const int POST_PASSES = 10;


class Renderer :
    public OGLRenderer
{
public:
    Renderer(Window& parent);
    virtual ~Renderer();

    void UpdateScene(float dt) override;
    void RenderScene() override;

protected:

    void DrawScene();
    void DrawPostProcess();
    void PresentScene();

    ////TODO: Should use a component but done as a function of Renderer here for simplicity;
    //void UpdateScene()

    void DrawShadowScene();
    
    //PostProcessing
    shared_ptr<RenderTargetTexture> renderTargetCam;
    shared_ptr<RenderTargetTexture> renderTargetCur;
    vector<PostProcessObject*> ppoList;

    PPO_VolumetricLighting ppoVolumetricLighting;
    PPO_GaussianBlur ppoGaussianBlur;
    PPO_Sobel ppoSobel;
    PPO_ToScreen ppoToScreen;
    PPO_DoubleVision ppoDoubleVision;
	PPO_ColourGrading ppoColourGrading;


};



