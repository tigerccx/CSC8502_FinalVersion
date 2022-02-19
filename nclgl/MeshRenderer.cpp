#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(): 
	ComponentManaged(), meshMaterial(nullptr), mesh(nullptr)//, boundingGeometry(BoundingGeometry(BoundingGeometryType_AABB))
{

}

MeshRenderer::MeshRenderer(shared_ptr<MeshMaterial> material, shared_ptr<Mesh> mesh):
	ComponentManaged(), meshMaterial(material)//, boundingGeometry(BoundingGeometry(BoundingGeometryType_AABB))
{
	SetMesh(mesh);
}

void MeshRenderer::OnActivated()
{
	GetGO()->transform->SetBoundingAABBMesh(mesh);
}

void MeshRenderer::OnDeactivated()
{
	GetGO()->transform->SetBoundingAABBMesh(nullptr);
}

void MeshRenderer::Update(float dt)
{
	if (meshAnimator) {
		meshAnimator->UpdateFrame(dt, mesh);
	}
}

void MeshRenderer::UpdateSkinnedBoundingGeometry()
{
	//TODO: More efficient to calculate bounding geometry in shaderPrePixelBump and store TransformFeedback result for later renderering...
	if (meshAnimator) {
		BoundingGeometry boundingAABB = meshAnimator->CalculateAnimatedMeshBoundingGeometry(mesh);
		GetGO()->transform->SetBoundingAABB(boundingAABB);
	}
}

void MeshRenderer::Render()
{
	BindMaterial();
	UpdateShaderModelMatrix();
	UpdateMeshAnimationData();
	mesh->Draw();
	UnindMaterial();
}

void MeshRenderer::RenderSubMesh(int i)
{
	BindMaterial(i);
	UpdateShaderModelMatrix(i);
	UpdateMeshAnimationData(i);
	mesh->DrawSubMesh(i);
	UnindMaterial(i);
}

void MeshRenderer::BindMaterial()
{
	auto material = meshMaterial->GetMaterial();
	material->BindShader();
	material->UpdateData();
}

void MeshRenderer::BindMaterial(int i)
{
	auto material = meshMaterial->GetMaterialForLayer(i);
	material->BindShader();
	material->UpdateData();
}

void MeshRenderer::UnindMaterial()
{
	auto material = meshMaterial->GetMaterial();
	material->UnbindData();
}

void MeshRenderer::UnindMaterial(int i)
{
	auto material = meshMaterial->GetMaterialForLayer(i);
	material->UnbindData();
}

void MeshRenderer::UpdateBoundingGeometry()
{
	if(GetGO())
		GetGO()->transform->SetBoundingAABBMesh(mesh);
}
