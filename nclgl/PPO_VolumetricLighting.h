#pragma once
#include <cassert>

#include "PostProcessObject.h"
#include "Camera.h"
#include "LightManager.h"

class PPO_VolumetricLighting :
    public PostProcessObject
{
public:    
    PPO_VolumetricLighting();
    virtual ~PPO_VolumetricLighting() = default;

    void SetRenderScale(float scale);
    inline void SetSteps(int steps) { this->steps = steps; }
    inline void SetStepLengthStart(int stepLength) { this->stepLength = stepLength; }
    inline void SetStrength(int strength) { this->strength = strength; }

    virtual void SetupPostProcessObject(int screenWidth, int screenHeight) override;
    virtual void RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast) override;

    Camera* camRender = nullptr;

protected:
    shared_ptr<Shader> shaderVolumetricLighting;
    shared_ptr<Shader> shaderAdd;
    shared_ptr<RenderTargetTexture> rtTemp = nullptr;

    int steps = 500;
    float stepLength = 0.2f;
    float strength = 0.01f;

    float scale = 1.0f;
    int screenWidth = 0;
    int screenHeight = 0;
};

