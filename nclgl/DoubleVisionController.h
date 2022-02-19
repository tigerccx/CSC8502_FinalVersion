#pragma once

#include "Component.h"
#include "PPO_DoubleVision.h"

using namespace std;

class DoubleVisionController :
    public Component
{
public:
    DoubleVisionController(PPO_DoubleVision* ppoDoubleVision, float rotSpeed = 10.0f, float scale = 100.0f, float scaleMin = 30.0f, float minBlend = 0.5f) :
        Component(), ppoDoubleVision(ppoDoubleVision), rotSpeed(rotSpeed), scaleOffset(scale), minBlend(minBlend), scaleMin(scaleMin){ }
    virtual ~DoubleVisionController() = default;

    virtual void Update(float dt) {
        tAccum += dt * rotSpeed;
        angle += sin(DegToRad(tAccum));
        while (angle > 360)
            angle = angle - 360;
        float scale = max(abs(cos(DegToRad(1.3*tAccum))) * scaleOffset, scaleMin);

        Vector2 offset = Vector2(cos(DegToRad(angle)), sin(DegToRad(angle))) * scale;

        ppoDoubleVision->SetDoubleVisionOffset(offset);
        ppoDoubleVision->SetDoubleVisionBlend(max(1 - scale / scaleOffset, minBlend));
    }

protected:
    PPO_DoubleVision* ppoDoubleVision;
    float rotSpeed = 60.0f;
    float scaleOffset = 100.0f;
    float minBlend = 0.5f;
    float scaleMin = 20.0f;

    float angle = 0;
    float tAccum = 0;
};

