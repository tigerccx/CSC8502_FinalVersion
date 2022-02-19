#pragma once

#include "PostProcessObject.h"

class PPO_GaussianBlur :
    public PostProcessObject
{
public:
    PPO_GaussianBlur();
    virtual ~PPO_GaussianBlur() = default;

    inline void SetCountPass(int countPass) { this->countPass = countPass; }

    virtual void SetupPostProcessObject(int screenWidth, int screenHeight) override;
    virtual void RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast) override;

protected:
    shared_ptr<RenderTargetTexture> renderTargetTmp0;
    shared_ptr<Shader> shaderBlur;
    int countPass = 10;
};

