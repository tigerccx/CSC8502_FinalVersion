#pragma once

#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

#include "Singleton.h"
#include "Light.h"
#include "Utility.h"

class LightManager: public Singleton<LightManager>
{
public:
	LightManager() = default;
	LightManager(const LightManager& other) = default;
	LightManager& operator=(const LightManager& other) = default;
	virtual ~LightManager();

	inline Light* CreateLight() { Light* lightNew = new Light(); lightList.emplace_back(lightNew); return lightNew; }
	bool DeleteLight(Light* light);
	const vector<Light*>& GetAllLights() const { return lightList; }
	void UploadLightsDataToShader(shared_ptr<Shader> shader);
	void UploadShadowDataToShader(shared_ptr<Shader> shader);


protected:
	vector<Light*> lightList;
};

