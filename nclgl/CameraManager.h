#pragma once

#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

#include "Singleton.h"
#include "Camera.h"

class CameraManager: public Singleton<CameraManager>
{
public:
	CameraManager() = default;
	CameraManager(const CameraManager& other) = default;
	CameraManager& operator=(const CameraManager& other) = default;
	virtual ~CameraManager();

	inline Camera* CreateCamera() { Camera* camNew = new Camera(); cameraList.emplace_back(camNew); return camNew; }
	bool DeleteCamera(Camera* camNew);

	void CreateCameraRenderOrderList();
	vector<Camera*>* GetCameraRenderOrderList() { return &cameraRenderOrderList; }

protected:
	vector<Camera*> cameraList;
	vector<Camera*> cameraRenderOrderList;
};

