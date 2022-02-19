#pragma once

#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <functional>

#include "Rendering_UniformBufferManager.h"
#include "Singleton.h"
#include "MeshRenderer.h"
#include "RenderTargetTexture.h"
#include "GameObjectManager.h"
#include "LightManager.h"
#include "CameraManager.h"

using namespace std;

struct RenderOrder {
	MeshRenderer* meshRenderer;
	int idxSubMesh;
};

struct DataPerInstancing {
	std::shared_ptr<Shader> shader = nullptr;
	std::shared_ptr<Mesh> mesh = nullptr;
	int idxSubMesh = -1;

	DataPerInstancing& operator=(const DataPerInstancing& other) = default;
	DataPerInstancing(const DataPerInstancing& other) = default;
	bool operator==(const DataPerInstancing& other) const {
		return (shader == other.shader) && (mesh == other.mesh) && (idxSubMesh == other.idxSubMesh);
	}
	bool operator>(const DataPerInstancing& other) const {
		return (shader > other.shader) || (mesh > other.mesh) || (idxSubMesh > other.idxSubMesh);
	}
	bool operator<(const DataPerInstancing& other) const {
		return (shader < other.shader) || (mesh < other.mesh) || (idxSubMesh < other.idxSubMesh);
	}
};

class RendererManager: public Singleton<RendererManager>
{
public:
	RendererManager() = default;
	RendererManager(const RendererManager& other) = default;
	RendererManager& operator=(const RendererManager& other) = default;
	~RendererManager();

	inline void SetCamera(Camera* camera) { this->camera = camera; SetCameraPos(camera->GetGO()->transform->GetPositionWorld()); }
	inline void SetCameraPos(const Vector3& posCam) { this->posCamera = posCam; }
	inline Camera* GetCamera() { return this->camera; }

	inline MeshRenderer* CreateRenderer() { MeshRenderer* rendererNew = new MeshRenderer(); rendererList.emplace_back(rendererNew); return rendererNew; }
	bool DeleteRenderer(MeshRenderer* renderer);

	void RenderAllCameras();
	void RenderWithCamera(Camera* camera);
	void RenderShadowMaps();
	void RenderShadowMapWithLight(Light* light);

protected:
	typedef std::function<void(MeshRenderer* renderer)> RendererHandler;
	
	typedef std::vector<RenderOrder> RenderersList;
	typedef std::map<std::shared_ptr<Shader>, RenderersList> RenderersPerShaderList;
	typedef std::map<DataPerInstancing, RenderersList> RenderersPerInstancingList;
	

	Camera* camera;
	Vector3 posCamera;
	vector<MeshRenderer*> rendererList;

	//Render Lists
	RenderersPerShaderList renderersPerShaderList;
	RenderersPerInstancingList renderersPerInstancingList;
	RenderersList renderersTransparentList;
	//Shadow Render Lists
	RenderersList renderersPerLight;
	RenderersPerInstancingList renderersInstancedPerLight;

	//Render Lists Management
	void BuildRendererListsWithCurrentCamera();
	void BuildRendererListsWithCamera(const CameraFrustum& frustum);
	void SortRendererLists();
	inline void ClearRendererLists();
	void __PerCamRendererHandler(MeshRenderer* renderer);
	void __CreateAndAddRenderOrder(MeshRenderer* renderer, shared_ptr<Mesh> mesh, int idxSubMesh, shared_ptr<Material> mat);
	
	//Shadow Render Lists Management
	void BuildRendererListsWithLight(const CameraFrustum& frustum);
	void SortRendererListsPerLight();
	inline void ClearRendererListsPerLight();
	void __PerLightRendererHandler(MeshRenderer* renderer);
	void __CreateAndAddRenderOrderForLight(MeshRenderer* renderer, int idxSubMesh);
	
	//Render List Management Helper
	void __ApplyOnRenderingRenderers(RendererHandler handler, const CameraFrustum& frustum);
	void __SortRenderOrderList(RenderersList::iterator& beg, RenderersList::iterator& end);
	void __SortRenderOrderListReverse(RenderersList::reverse_iterator& rbeg, RenderersList::reverse_iterator& rend);

	//Render
	void RenderShader(shared_ptr<Shader> shader);
	void RenderShader(shared_ptr<Shader> shader, RenderersList& renderers);
	void RenderMeshRenderer(MeshRenderer* meshRenderer, int idxSubMesh);
	void RenderInstanced(const DataPerInstancing& data, RenderersList& renderers);
	void RenderShadowShader(Light* light);
	void RenderShadowShaderInstanced(Light* light);
	//Render Helper
	void __PreprocessLightingForRenderShader(shared_ptr<Shader> shader);
	
	void UpdateSkinnedBoundingGeometry();

	void ClearCameraRenderTarget();

	void SetCameraRenderTarget();
	void UnsetCameraRenderTarget() {
		RenderTargetTexture::SetDefaultRenderTarget();
	}
	void SetupCamera() {
		SetupCameraColourWriting();
		SetupCameraDepthTestingAndWriting();
		SetupCameraCulling();
	}
	void SetupCameraDepthTestingAndWriting();
	void SetupCameraCulling();
	void SetupCameraColourWriting();
};

