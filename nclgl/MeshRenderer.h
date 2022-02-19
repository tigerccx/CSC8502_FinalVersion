#pragma once

#include <array>

#include "ComponentManaged.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshMaterial.h"
#include "MeshAnimator.h"
#include "BoundingGeometry.h"
#include "Camera.h"
#include "Frustum.h"
#include "Geometry.h"

#define RENDER_ORDER_PREOPAQUE -1000
#define RENDER_ORDER_OPAQUE 0
#define RENDER_ORDER_TRANSPARENT 1000

using namespace std;

class RendererManager;

class MeshRenderer :
	public ComponentManaged
{
public:
	friend class RendererManager;

	MeshRenderer();
	MeshRenderer(shared_ptr<MeshMaterial> material, shared_ptr<Mesh> mesh);
	virtual ~MeshRenderer() = default;

	int renderOrder = 0; //Smaller->Render First  //TODO: Should be per shader level. But for simplicity...
	bool alwaysRender = false; //No matter whether in sight, always render. NOTE: won't render if renderer is not active.
	bool castShadow = false;
	bool enableInstancing = false; //So far only support transform instancing //TODO: Support material instancing

	virtual void OnActivated() override;
	virtual void OnDeactivated() override;
	virtual void Update(float dt) override;

	void UpdateSkinnedBoundingGeometry();

	void Render();
	void RenderSubMesh(int i);

	inline void SetMeshMaterial(shared_ptr<MeshMaterial> newMaterial) { meshMaterial = newMaterial; }
	inline shared_ptr<MeshMaterial> GetMeshMaterial() const { return meshMaterial; }
	inline void SetMesh(shared_ptr<Mesh> newMesh) { mesh = newMesh; UpdateBoundingGeometry(); BufferMesh(); }
	inline shared_ptr<Mesh> GetMesh() const { return mesh; }
	inline void SetMeshAnimator(shared_ptr<MeshAnimator> newMeshAnimator) { meshAnimator = newMeshAnimator; } //TODO: Need to somehow ensure MeshAnimator is not owned by multiple objects!!!
	inline shared_ptr<MeshAnimator> GetMeshAnimator() const { return meshAnimator; }

	static inline bool CompareCameraDistance(MeshRenderer* a, MeshRenderer* b, const Vector3& posCam) {
		return (a->GetGO()->transform->GetPositionWorld()- posCam).SqrLength()<
				(b->GetGO()->transform->GetPositionWorld() - posCam).SqrLength();
	}
	static inline bool CompareRenderOrder(MeshRenderer* a, MeshRenderer* b) {
		return a->renderOrder <= b->renderOrder;
	}

	void UploadModelMatrixToShader(shared_ptr<Shader> shader) {
		glUniformMatrix4fv(
			glGetUniformLocation(shader->GetProgram(), "modelMatrix"),
			1, false, gameobject->transform->GetModelMatrix().values);
	}

protected:
	shared_ptr<MeshMaterial> meshMaterial = nullptr;
	shared_ptr<Mesh> mesh = nullptr;
	shared_ptr<MeshAnimator> meshAnimator = nullptr;

	inline void UpdateShaderModelMatrix() { UpdateShaderModelMatrix(-1); }

	void UpdateShaderModelMatrix(int i){
		if (meshMaterial) {
			shared_ptr<Material> mat = nullptr;
			if(i<0)
				mat = meshMaterial->GetMaterial();
			else
				mat = meshMaterial->GetMaterialForLayer(i);
			if (mat) {
				auto shader = mat->GetShader();
				if(shader)
					glUniformMatrix4fv(
						glGetUniformLocation(shader->GetProgram(), "modelMatrix"),
						1, false, gameobject->transform->GetModelMatrix().values);
				else
					throw NullPtrException();
			}
			else
				throw NullPtrException();
		}
		else
			throw NullPtrException();
	}

	inline void UpdateMeshAnimationData() { UpdateMeshAnimationData(-1); }

	void UpdateMeshAnimationData(int i) {
		if (meshMaterial) {
			shared_ptr<Material> mat = nullptr;
			if (i < 0)
				mat = meshMaterial->GetMaterial();
			else
				mat = meshMaterial->GetMaterialForLayer(i);
			if (mat) {
				auto shader = mat->GetShader();
				if (shader) {
					if (meshAnimator) {
						meshAnimator->UpdateRenderStateToShader(shader);
					}
				}
				else
				{
					throw NullPtrException();
				}
			}
			else
				throw NullPtrException();
		}
		else
			throw NullPtrException();
	}

	void BindMaterial();
	void BindMaterial(int i);
	void UnindMaterial();
	void UnindMaterial(int i);
	inline void BufferMesh() { mesh->BufferData(); }
	void UpdateBoundingGeometry();
};

