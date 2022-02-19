#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "PostProcessObject.h"

class GameController :
    public Component
{
public:
    Transform* transInControl = nullptr;
    
    
    Camera* mainCam;
    shared_ptr<Texture> cubemapNight;
    shared_ptr<Material> matWater;


    GameObject* goMirror;
    GameObject* goDirectionalLight;
    Light* lightDirection;
    GameObject* goSpotlight;
    Light* lightSpot;
    vector<GameObject*> goPointlights;
    vector<Light*> lightsPoint;


    PostProcessObject* ppoVolumetricLighting;
	PostProcessObject* ppoGaussian;
	PostProcessObject* ppoDoubleVision;
	PostProcessObject* ppoColourGrading;

    virtual void Update(float dt) override;
};

