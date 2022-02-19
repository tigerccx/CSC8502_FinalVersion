#pragma once
#include "PostProcessObject.h"
class PPO_DoubleVision :
    public PostProcessObject
{
public:
    PPO_DoubleVision();
    virtual ~PPO_DoubleVision() = default;
    
    virtual void SetupPostProcessObject(int screenWidth, int screenHeight) override;
    virtual void RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast) override;

    inline void SetDoubleVisionOffset(Vector2 offset) { this->offset = offset; }
    inline void SetDoubleVisionBlend(float blend) { this->blend = blend; }

protected:
    shared_ptr<Shader> shaderDoubleVision;
    Vector2 offset;
    float blend = 0.8;
};

