#pragma once

#include "PostProcessObject.h"

class PPO_ColourGrading :
    public PostProcessObject
{
public:
	PPO_ColourGrading();
    virtual ~PPO_ColourGrading() = default;

    inline void SetStages(int stages) { this->stages = stages; }

    virtual void SetupPostProcessObject(int screenWidth, int screenHeight) override;
    virtual void RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast) override;

protected:
    shared_ptr<Shader> shaderGrading;
    int stages = 10;
};

