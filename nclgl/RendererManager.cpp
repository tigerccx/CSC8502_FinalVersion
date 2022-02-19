#include "RendererManager.h"

RendererManager::~RendererManager()
{
	while (!rendererList.empty()) {
		MeshRenderer* renderer = rendererList[rendererList.size() - 1];
		if (renderer)
			delete renderer;
		rendererList.pop_back();
	}
}

void RendererManager::SetupCameraCulling()
{
	if (camera->GetFaceCulling()) {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}
	glCullFace(camera->GetFaceCullingType());
	glFrontFace(camera->faceWindingOrder);
}

void RendererManager::SetupCameraColourWriting()
{
	bool r, g, b, a;
	camera->GetColourMask(r, g, b, a);
	glColorMask(r, g, b, a);
}

void RendererManager::SetupCameraDepthTestingAndWriting()
{
	if (camera->GetDepthTest()) {
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
	glDepthMask(camera->GetDepthWrite());
	glDepthFunc(camera->GetDepthFunc());
}

void RendererManager::SetCameraRenderTarget()
{
	shared_ptr<RenderTargetTexture> renderTarget = camera->GetCameraRenderTarget();
	if (renderTarget) {
		renderTarget->SetAsRenderTarget();
	}
	else
	{
		RenderTargetTexture::SetDefaultRenderTarget();
	}
}

void RendererManager::ClearCameraRenderTarget()
{
	auto clearType = camera->GetClearType();
	switch (clearType)
	{
	case CameraClearType_No:
		break;
	case CameraClearType_Colour: {
		Vector4 clearColor = camera->GetClearColor();
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(camera->GetClearFlag());
	}
							   break;
	case CameraClearType_Skybox: {
		Vector4 clearColor = camera->GetClearColor();
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(camera->GetClearFlag());
		camera->RenderSkybox();
	}
							   break;
	default:
		break;
	}
}

bool RendererManager::DeleteRenderer(MeshRenderer* renderer)
{
	auto itr = find(rendererList.begin(), rendererList.end(), renderer);
	if (itr == rendererList.end()) {
		return false;
	}
	else
	{
		delete* itr;
		itr = rendererList.erase(itr);
		return true;
	}
}

void RendererManager::UpdateSkinnedBoundingGeometry()
{
	for (auto& renderer : rendererList) {
		auto meshAnimator = renderer->GetMeshAnimator();
		if (meshAnimator)
			renderer->UpdateSkinnedBoundingGeometry();
	}
}

//Build renderer list specifiying renderer renderMode(common/transparent)
//(and submesh idx)
void RendererManager::BuildRendererListsWithCurrentCamera()
{
	CameraParamPerspective camParam;
	camera->GetCameraParameters(camParam);
	CameraFrustum frustum = CameraFrustum(camera->GetGO()->transform->GetModelMatrix(), camParam.fov, camParam.aspectWH, camParam.nearPlane, camParam.farPlane);
	BuildRendererListsWithCamera(frustum);
}

void RendererManager::BuildRendererListsWithCamera(const CameraFrustum& frustum)
{
	__ApplyOnRenderingRenderers(std::bind(&RendererManager::__PerCamRendererHandler, this, placeholders::_1),
		frustum);
}

void RendererManager::BuildRendererListsWithLight(const CameraFrustum& frustum) 
{
	__ApplyOnRenderingRenderers(std::bind(&RendererManager::__PerLightRendererHandler, this, placeholders::_1),
								frustum);
}

void RendererManager::SortRendererListsPerLight()
{
	__SortRenderOrderList(renderersPerLight.begin(), renderersPerLight.end());
}

void RendererManager::ClearRendererListsPerLight()
{
	renderersPerLight.clear();
	renderersInstancedPerLight.clear();
}

//Sort to renderers: 
//1. sort opaque per shader;
//2. sort transparent globally;
void RendererManager::SortRendererLists()
{
	//Opaque
	for (auto &itr : renderersPerShaderList) {
		__SortRenderOrderList(itr.second.begin(), itr.second.end());
	}
	//Transparent
	__SortRenderOrderListReverse(renderersTransparentList.rbegin(), renderersTransparentList.rend());
}

void RendererManager::ClearRendererLists()
{
	renderersPerShaderList.clear();
	renderersTransparentList.clear();
	renderersPerInstancingList.clear();
}

void RendererManager::__CreateAndAddRenderOrder(MeshRenderer* renderer, shared_ptr<Mesh> mesh, int idxSubMesh, shared_ptr<Material> mat)
{
	if (!mat) return;
	shared_ptr<Shader> shader = mat->GetShader();
	if (!shader) return;

	RenderOrder ro;
	ro.meshRenderer = renderer;
	ro.idxSubMesh = idxSubMesh;

	if (mat->renderMode == MaterialRenderMode_Transparent) { //If transparent
		renderersTransparentList.emplace_back(ro);
	}
	else
	{
		if (renderer->enableInstancing) { //If instancing
			DataPerInstancing data = {shader, mesh, idxSubMesh};
			renderersPerInstancingList[data].emplace_back(ro);
		}
		else {
			renderersPerShaderList[shader].emplace_back(ro);
		}
			
	}
}

void RendererManager::__PerCamRendererHandler(MeshRenderer* renderer)
{
	GameObject* go = renderer->GetGO();

	shared_ptr<MeshMaterial> meshMat = renderer->GetMeshMaterial();
	shared_ptr<Mesh> mesh = renderer->GetMesh();
	int countSubMeshes = mesh->GetSubMeshCount();

	//TODO: Should somehow let MeshRender manage if the material is suited for the mesh
	if (countSubMeshes > 0) { //If mesh has submeshes
		for (int idxSubMesh = 0; idxSubMesh < countSubMeshes; ++idxSubMesh) {

			shared_ptr<Material> mat = meshMat->GetMaterialForLayer(idxSubMesh);

			if (mat)
				__CreateAndAddRenderOrder(renderer, mesh, idxSubMesh, mat);
			else
				throw NullPtrException();

		}
	}
	else {
		shared_ptr<Material> mat = meshMat->GetMaterial();
		if (mat)
			__CreateAndAddRenderOrder(renderer, mesh, -1, mat);
		else
			throw NullPtrException();
	}
}

void RendererManager::__CreateAndAddRenderOrderForLight(MeshRenderer* renderer, int idxSubMesh)
{
	shared_ptr<Mesh> mesh = renderer->GetMesh();
	int countSubMeshes = mesh->GetSubMeshCount();

	RenderOrder ro;
	ro.meshRenderer = renderer;
	ro.idxSubMesh = idxSubMesh;

	//TODO: potentially add semi-transparent shadowing...?
	if (renderer->enableInstancing) { //If instancing
		DataPerInstancing data = { nullptr, mesh, idxSubMesh };
		renderersInstancedPerLight[data].emplace_back(ro);
	}
	else {
		renderersPerLight.emplace_back(ro);
	}
}

void RendererManager::__PerLightRendererHandler(MeshRenderer* renderer)
{
	if (!renderer->castShadow)
		return;

	GameObject* go = renderer->GetGO();

	shared_ptr<Mesh> mesh = renderer->GetMesh();
	int countSubMeshes = mesh->GetSubMeshCount();

	if (countSubMeshes > 0) { //If mesh has submeshes
		for (int idxSubMesh = 0; idxSubMesh < countSubMeshes; ++idxSubMesh) {
			__CreateAndAddRenderOrderForLight(renderer, idxSubMesh);
		}
	}
	else {
		__CreateAndAddRenderOrderForLight(renderer, -1);
	}
}

void RendererManager::__ApplyOnRenderingRenderers(RendererHandler handler, const CameraFrustum& frustum)
{
	Transform* transCur = nullptr;
	queue<Transform*> queueTrans;
	queueTrans.emplace(&Transform::root);
	vector<Transform*> children;
	MeshRenderer* renderer;

	while (!queueTrans.empty()) {
		transCur = queueTrans.front();
		queueTrans.pop();
		auto go = transCur->GetGO();
		if (go) {
			renderer = go->GetComponent<MeshRenderer>();
		}
		else
		{
			renderer = nullptr;
		}

		transCur->GetChildren(children);
		for (Transform* transChild : children) {
			queueTrans.emplace(transChild);
		}

		bool toRenderObject = renderer != nullptr && renderer->IsActive();
		toRenderObject = toRenderObject && (renderer->IsActive() || renderer->alwaysRender);
		if (toRenderObject) {
			//Add children to queue

			//If node has renderer, add render
			//( and which submesh to render since now we are using MeshMaterial(which contains multiple materials))
			if (transCur == &Transform::root)
				continue;

			if (toRenderObject) {
				handler(renderer);
			}
		}

	}
}

void RendererManager::__SortRenderOrderList(RenderersList::iterator& beg, RenderersList::iterator& end)
{
	std::sort(beg, end,
		[&](RenderOrder a, RenderOrder b) {
			return (a.meshRenderer == b.meshRenderer) || 
				(MeshRenderer::CompareRenderOrder(a.meshRenderer, b.meshRenderer) && 
				MeshRenderer::CompareCameraDistance(a.meshRenderer, b.meshRenderer, posCamera));
		}
	);
}

void RendererManager::__SortRenderOrderListReverse(RenderersList::reverse_iterator& rbeg, RenderersList::reverse_iterator& rend)
{
	std::sort(rbeg, rend,
		[&](RenderOrder a, RenderOrder b) {
			return (a.meshRenderer == b.meshRenderer) || 
				(MeshRenderer::CompareRenderOrder(a.meshRenderer, b.meshRenderer) && 
				MeshRenderer::CompareCameraDistance(a.meshRenderer, b.meshRenderer, posCamera));
		}
	);
}

void RendererManager::RenderAllCameras()
{
	CameraManager::GetInstance()->CreateCameraRenderOrderList();
	auto cameraList = CameraManager::GetInstance()->GetCameraRenderOrderList();
	for(Camera* camCur: *cameraList){
		RenderWithCamera(camCur);
	}
}

void RendererManager::RenderWithCamera(Camera* camera)
{
	SetCamera(camera);
	SetCameraRenderTarget();
	SetupCameraCulling();
	ClearCameraRenderTarget();
	UpdateSkinnedBoundingGeometry();

	GameObjectManager::GetInstance()->PrePerCameraDraw(camera);
	

	BuildRendererListsWithCurrentCamera();
	SortRendererLists();
	
	//Upload cam data
	Rendering_UniformBufferObject ubo;
	if (!Rendering_UniformBufferManager::GetInstance()->GetUBO(ubo)) {
		throw out_of_range("Ran out of UBOs somehow...");
	}
	CameraShaderData cameraData = { camera->BuildViewMatrix(), camera->projMatrix };
	glBindBuffer(GL_UNIFORM_BUFFER, ubo.buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(cameraData), &cameraData, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, NCLGL_SHADER_INSTANCING_UB_CAMERA_DATA_BP, ubo.buffer);

	//Common Rendering
	for (auto& itr : renderersPerShaderList) {
		RenderShader(itr.first, itr.second);
	}
	//Instace Rendering
	for (auto& itr : renderersPerInstancingList) {
		RenderInstanced(itr.first, itr.second);
	}
	//Transparent Rendering
	for (auto& itr : renderersTransparentList) {
		RenderMeshRenderer(itr.meshRenderer, itr.idxSubMesh);
	}

	Rendering_UniformBufferManager::GetInstance()->ReturnUBO(ubo);

	ClearRendererLists();

	GameObjectManager::GetInstance()->PostPerCameraDraw(camera);

	UnsetCameraRenderTarget();
}

void RendererManager::RenderShadowMaps()
{
	const vector<Light*>& lightList = LightManager::GetInstance()->GetAllLights();

	for (int i = 0; i < lightList.size(); ++i) {
		Light* light = lightList[i];
		if (!light->IsActive())
			continue;
		if (light->GetCastShadow()) {
			RenderShadowMapWithLight(light);
		}
	}
}

void RendererManager::RenderShadowMapWithLight(Light* light)
{
	Transform* transformLight = light->GetGO()->transform;
	SetCameraPos(transformLight->GetPositionWorld());

	switch (light->GetLightType())
	{
	case LightType_Directional: {
		light->GetShadowMap()->SetAsRenderTarget();
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);
		int shadowMapSize = light->GetShadowMapSize();
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glViewport(0, 0, shadowMapSize, shadowMapSize);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		Vector3 posW = DIRECTIONAL_CENT; //TODO: Centerize Dlight
		auto rot = light->GetGO()->transform->GetRotationWorld().ToEuler();
		Matrix4 modelMatrix = Matrix4::Translation(posW)*
		(Matrix4::Rotation(light->GetGO()->transform->GetRotationWorld())
			* Matrix4::Scale(light->GetGO()->transform->GetScaleLocal()));
		BuildRendererListsWithLight(CameraFrustum(modelMatrix, DIRECTIONAL_NEAR, DIRECTIONAL_FAR, -DIRECTIONAL_RANGE, DIRECTIONAL_RANGE, -DIRECTIONAL_RANGE, DIRECTIONAL_RANGE));
		SortRendererListsPerLight();

		light->UploadViewProjMatrixToShadowShader();
		RenderShadowShader(light);
		light->UploadViewProjMatrixToShadowShader(0, true);
		RenderShadowShaderInstanced(light);
		ClearRendererListsPerLight();

		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		RenderTargetTexture::SetDefaultRenderTarget();
	}
		break;
	case LightType_Point: {
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		int shadowMapSize = light->GetShadowMapSize();
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0, 0, shadowMapSize, shadowMapSize);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		PointLightParameters param = light->GetPointLightParameters();
		Vector3 posW = light->GetGO()->transform->GetPositionWorld();

		auto shadowCubeMap = light->GetShadowCubeMap();

		for (int i = 0; i < 6; ++i) {
			shadowCubeMap->SetAsRenderTarget((CubeTextureSide)i);
			glClear(GL_DEPTH_BUFFER_BIT);

			Matrix4 modelMatrix;
			Quaternion q;
			switch (i)
			{
			case CubeTextureSide_Front: {
				q = Quaternion::FromEulerAngles(0, 180, 0);
			}
				break;
			case CubeTextureSide_Back: {
				q = Quaternion::FromEulerAngles(0, 0, 0);
			}
				break;
			case CubeTextureSide_Right: {
				q = Quaternion::FromEulerAngles(0, -90, 0);
			}
				break;
			case CubeTextureSide_Left: {
				q = Quaternion::FromEulerAngles(0, 90, 0);
			}
				break;
			case CubeTextureSide_Top: {
				q = Quaternion::FromEulerAngles(-90, 0, 0);
			}
				break;
			case CubeTextureSide_Bottom: {
				q = Quaternion::FromEulerAngles(90, 0, 0);
			}
				break;
			default:
				break;
			}

			modelMatrix = Matrix4::Translation(transformLight->GetPositionWorld())
				* (Matrix4::Rotation(q)
					* Matrix4::Scale(transformLight->GetScaleLocal()));

			BuildRendererListsWithLight(CameraFrustum(modelMatrix, 90, 1, 0.1f, param.radius));
			SortRendererListsPerLight();

			auto shadowShaderPointLight = Light::GetPointLightShadowShader();
			glUseProgram(shadowShaderPointLight->GetProgram());
			glUniform1f(glGetUniformLocation(shadowShaderPointLight->GetProgram(), "lightRad"), param.radius);
			glUniform3fv(glGetUniformLocation(shadowShaderPointLight->GetProgram(), NCLGL_SHADER_LIGHT_POSITION), 1, (float*)&posW);
			light->UploadViewProjMatrixToShadowShader(i);
			RenderShadowShader(light);

			auto shadowShaderPointLightInstanced = Light::GetPointLightShadowShaderInstanced();
			glUseProgram(shadowShaderPointLightInstanced->GetProgram());
			glUniform1f(glGetUniformLocation(shadowShaderPointLightInstanced->GetProgram(), "lightRad"), param.radius);
			glUniform3fv(glGetUniformLocation(shadowShaderPointLightInstanced->GetProgram(), NCLGL_SHADER_LIGHT_POSITION), 1, (float*)&posW);
			light->UploadViewProjMatrixToShadowShader(i, true);
			RenderShadowShaderInstanced(light);
			ClearRendererListsPerLight();

			shadowCubeMap->ClearRenderTargetBinding((CubeTextureSide)i);
		}

		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		shadowCubeMap->SetDefaultRenderTarget();
	}
		break;
	case LightType_Spot: {
		light->GetShadowMap()->SetAsRenderTarget();
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);
		int shadowMapSize = light->GetShadowMapSize();
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glViewport(0, 0, shadowMapSize, shadowMapSize);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		glUseProgram(Light::GetShadowShader()->GetProgram());

		SpotLightParameters param = light->GetSpotLightParameters();
		Vector3 posW = light->GetGO()->transform->GetPositionWorld();
		BuildRendererListsWithLight(CameraFrustum(light->GetGO()->transform->GetModelMatrix(), param.angle, 1, 0.1f, param.radius));
		SortRendererListsPerLight();
		light->UploadViewProjMatrixToShadowShader();
		RenderShadowShader(light);
		light->UploadViewProjMatrixToShadowShader(0, true);
		RenderShadowShaderInstanced(light);
		ClearRendererListsPerLight();

		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		RenderTargetTexture::SetDefaultRenderTarget();
	}
		break;
	default:
		break;
	}
}

void RendererManager::RenderShader(shared_ptr<Shader> shader)
{
	auto itr0 = renderersPerShaderList.find(shader);
	if (itr0 == renderersPerShaderList.end()) {
		return;
	}

	RenderShader(shader, itr0->second);
}

void RendererManager::RenderShader(shared_ptr<Shader> shader, RenderersList& renderers)
{
	glUseProgram(shader->GetProgram());
	camera->UpdateMatrixToShader(shader);

	__PreprocessLightingForRenderShader(shader);

	for (const auto& itr : renderers) {
		auto meshRenderer = itr.meshRenderer;
		int idxSubMesh = itr.idxSubMesh;
		if (idxSubMesh >= 0)
			meshRenderer->RenderSubMesh(idxSubMesh);
		else
			meshRenderer->Render();
		GameObject* gotmp = meshRenderer->GetGO();
		string str1 = gotmp->name;
	}
}

void RendererManager::RenderMeshRenderer(MeshRenderer* meshRenderer, int idxSubMesh)
{
	auto mr = meshRenderer->GetMeshMaterial();
	if (!mr) {
		throw NullPtrException();
	}
	shared_ptr<Material> mat;
	if (idxSubMesh >= 0) {
		mat = mr->GetMaterialForLayer(idxSubMesh);
	}
	else
	{
		mat = mr->GetMaterial();
	}
	if (!mat) {
		throw NullPtrException();
	}
	auto shader = mat->GetShader();
	if (!shader) {
		throw NullPtrException();
	}
	glUseProgram(shader->GetProgram());
	camera->UpdateMatrixToShader(shader);
	__PreprocessLightingForRenderShader(shader);
	if (idxSubMesh >= 0)
		meshRenderer->RenderSubMesh(idxSubMesh);
	else
		meshRenderer->Render();
}

void RendererManager::RenderInstanced(const DataPerInstancing& data, RenderersList& renderers)
{
	//TODO: potenially add support for material instancing. For now, just gonna use the first material available. 

	shared_ptr<Shader> shader = data.shader;
	if (!shader) {
		throw NullPtrException();
	}
	glUseProgram(shader->GetProgram());
	camera->UpdateMatrixToShader(shader);

	__PreprocessLightingForRenderShader(shader);

	std::vector<Matrix4> uniformBlockData;
	for (auto& renderer : renderers) {
		GameObject* go = renderer.meshRenderer->GetGO();
		uniformBlockData.emplace_back(go->transform->GetModelMatrix());
	}

	Rendering_UniformBufferObject ubo;
	if (!Rendering_UniformBufferManager::GetInstance()->GetUBO(ubo)) {
		throw out_of_range("Ran out of UBOs somehow...");
	}

	glBindBuffer(GL_UNIFORM_BUFFER, ubo.buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4)* uniformBlockData.size(), uniformBlockData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	glBindBufferBase(GL_UNIFORM_BUFFER, NCLGL_SHADER_INSTANCING_UB_INSTANCE_BP, ubo.buffer);
	auto uniformBlockIndex = glGetUniformBlockIndex(shader->GetProgram(), NCLGL_SHADER_INSTANCING_UB_INSTANCE);
	if (uniformBlockIndex== GL_INVALID_INDEX) {
		std::cout << "Cannot enable instancing! Unsupported shader!" << std::endl;
		throw NotImplementedException();
	}
	glUniformBlockBinding(shader->GetProgram(), uniformBlockIndex, NCLGL_SHADER_INSTANCING_UB_INSTANCE_BP);

	if (data.idxSubMesh >= 0) {
		renderers[0].meshRenderer->BindMaterial(data.idxSubMesh);
		data.mesh->DrawSubMeshInstanced(data.idxSubMesh, renderers.size());
		renderers[0].meshRenderer->UnindMaterial(data.idxSubMesh);
	}
	else
	{
		renderers[0].meshRenderer->BindMaterial();
		data.mesh->DrawInstanced(renderers.size());
		renderers[0].meshRenderer->UnindMaterial();
	}

	Rendering_UniformBufferManager::GetInstance()->ReturnUBO(ubo);
}

void RendererManager::RenderShadowShader(Light* light)
{
	shared_ptr<Shader> shader;
	if (light->GetLightType() == LightType_Point) {
		shader = Light::GetPointLightShadowShader();
	}
	else
	{
		shader = Light::GetShadowShader();
	}
	
	glUseProgram(shader->GetProgram());
	for (const auto& itr : renderersPerLight) {
		auto meshRenderer = itr.meshRenderer;
		int idxSubMesh = itr.idxSubMesh;
		meshRenderer->UploadModelMatrixToShader(shader);
		if (idxSubMesh >= 0)
			meshRenderer->GetMesh()->DrawSubMesh(idxSubMesh);
		else
			meshRenderer->GetMesh()->Draw();
	}
}

void RendererManager::RenderShadowShaderInstanced(Light* light)
{
	shared_ptr<Shader> shader;
	if (light->GetLightType() == LightType_Point) {
		shader = Light::GetPointLightShadowShaderInstanced();
	}
	else
	{
		shader = Light::GetShadowShaderInstanced();
	}

	glUseProgram(shader->GetProgram());
	for (auto& perInstance : renderersInstancedPerLight) {

		std::vector<Matrix4> uniformBlockData;
		for (auto& renderer : perInstance.second) {
			GameObject* go = renderer.meshRenderer->GetGO();
			uniformBlockData.emplace_back(go->transform->GetModelMatrix());
		}

		Rendering_UniformBufferObject ubo;
		if (!Rendering_UniformBufferManager::GetInstance()->GetUBO(ubo)) {
			throw out_of_range("Ran out of UBOs somehow...");
		}

		glBindBuffer(GL_UNIFORM_BUFFER, ubo.buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4) * uniformBlockData.size(), uniformBlockData.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferBase(GL_UNIFORM_BUFFER, NCLGL_SHADER_INSTANCING_UB_INSTANCE_BP, ubo.buffer);
		auto uniformBlockIndex = glGetUniformBlockIndex(shader->GetProgram(), NCLGL_SHADER_INSTANCING_UB_INSTANCE);
		if (uniformBlockIndex == GL_INVALID_INDEX) {
			std::cout << "Cannot enable instancing! Unsupported shader!" << std::endl;
			throw NotImplementedException();
		}
		glUniformBlockBinding(shader->GetProgram(), uniformBlockIndex, NCLGL_SHADER_INSTANCING_UB_INSTANCE_BP);

		if (perInstance.first.idxSubMesh >= 0) {
			perInstance.first.mesh->DrawSubMeshInstanced(perInstance.first.idxSubMesh, perInstance.second.size());
		}
		else
		{
			perInstance.first.mesh->DrawInstanced(perInstance.second.size());
		}

		Rendering_UniformBufferManager::GetInstance()->ReturnUBO(ubo);
	}
}

void RendererManager::__PreprocessLightingForRenderShader(shared_ptr<Shader> shader)
{
	//TODO: Upload data into buffer so it won't need to be uploaded per shader
	//If shaderPrePixelBump is lighted, upload lighting data
	if (shader->IsLighted()) {
		LightManager::GetInstance()->UploadLightsDataToShader(shader);
		camera->UpdatePositionToLightedShader(shader);
		if (shader->IsShadowed()) {
			LightManager::GetInstance()->UploadShadowDataToShader(shader);
		}
	}
}
