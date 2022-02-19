#pragma once
#include "PostProcessObject.h"
class PPO_ToScreen :
    public PostProcessObject
{
public:
    PPO_ToScreen(): PostProcessObject(){};
    virtual ~PPO_ToScreen() = default;
    
    virtual void SetupPostProcessObject(int screenWidth, int screenHeight) override {};
    virtual void RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast) override;
};

