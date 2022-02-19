#pragma once
#include "Component.h"
#include "GameObject.h"
#include "MeshRenderer.h"

class WaterAnimator :
	public Component
{
public:
	WaterAnimator():Component(){}
	virtual ~WaterAnimator() = default;

	Camera* cam;
	shared_ptr<Material> mat;
	float speed0 = 0.001;
	float speed1 = 0.012;
	float rotSpeed1 = 0.3;

	virtual void Update(float dt) override;
	virtual void PreRender() override;

protected:
	float waterCycle = 0.0f;
	float waterCycle1 = 0.0f;
	float waterRotate1 = 0.0f;
	
};

