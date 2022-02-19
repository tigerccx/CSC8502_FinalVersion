#include "CameraManager.h"

CameraManager::~CameraManager()
{
	while (!cameraList.empty()) {
		Camera* cam = cameraList[cameraList.size() - 1];
		if (cam)
			delete cam;
		cameraList.pop_back();
	}
}

bool CameraManager::DeleteCamera(Camera* camNew)
{
	auto itr = find(cameraList.begin(), cameraList.end(), camNew);
	if (itr == cameraList.end()) {
		return false;
	}
	else
	{
		delete* itr;
		itr = cameraList.erase(itr);
		return true;
	}
}

void CameraManager::CreateCameraRenderOrderList()
{
	cameraRenderOrderList = cameraList;
	sort(cameraRenderOrderList.begin(),
		cameraRenderOrderList.end(),
		[&](Camera* a, Camera* b) { return a->GetRenderOrder() < b->GetRenderOrder(); }
	);
}
