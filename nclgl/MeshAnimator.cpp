#include "MeshAnimator.h"

shared_ptr<Shader> MeshAnimator::shaderPreCalc = nullptr;

void MeshAnimator::UpdateFrame(float dt, shared_ptr<Mesh> mesh)
{
	if (!meshAnimation)
		return;

	if (isPaused)
		return;


	//Update frame time and current frame
	frameTime -= speedScale * dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % meshAnimation->GetFrameCount();
		frameTime += 1.0f / meshAnimation->GetFrameRate();
	}
	float by = 1 - frameTime * meshAnimation->GetFrameRate();
	int nextFrame = (currentFrame + 1) % meshAnimation->GetFrameCount();

	//Update frame matrices
	frameMatrices.clear(); //TODO: not efficient! 
	const Matrix4* invBindPose = mesh->GetInverseBindPose(); //Matrices used to inverse model from bind pose to normal pose.
	const Matrix4* frameData = meshAnimation->GetJointData(currentFrame); //Get all the joint transforms in current frame
	const Matrix4* frameDataNext = meshAnimation->GetJointData(nextFrame);

	for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
		frameMatrices.emplace_back(Matrix4::InterpolateRotationAndTranslation(frameData[i], frameDataNext[i], by) * invBindPose[i]); //calculate transform of: inversing to normal and then applying animation
		//frameMatrices.emplace_back(Matrix4::InterpolateRotationAndTranslation(frameData[i]* invBindPose[i], frameDataNext[i]* invBindPose[i], by));
	}
}

void MeshAnimator::UpdateRenderStateToShader(shared_ptr<Shader> shader)
{
	if (!meshAnimation)
		return;
	int j = glGetUniformLocation(shader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
}

void MeshAnimator::InitPreCalcShader()
{
	if (!shaderPreCalc) {
		TransformFeedbackParam param;
		param.feedbackVaryings.emplace_back((char*)"positionSkinned");
		shaderPreCalc = ShaderManager::GetInstance()->CreateShader("SkinningVertex_PreCalc.glsl",
			"", "", "", "", true, param);
		if (!shaderPreCalc->LoadSuccess()) {
			throw LoadErrorException();
		}
	}
}

BoundingGeometry MeshAnimator::CalculateAnimatedMeshBoundingGeometry(shared_ptr<Mesh> mesh)
{
	InitPreCalcBuffer(mesh);

	glEnable(GL_RASTERIZER_DISCARD);
	glUseProgram(shaderPreCalc->GetProgram());

	UpdateRenderStateToPreCalcShader();

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, bufferPreCalc);

	BoundingGeometry bb = BoundingGeometry(BoundingGeometryType_AABB);
	int subMeshCount = mesh->GetSubMeshCount();
	if (subMeshCount == 0) {
		glBeginTransformFeedback(mesh->GetRenderMode());
		mesh->Draw();
		glEndTransformFeedback();
		glFlush();

		verticesOut.resize(mesh->GetNumVerticesToShader(-1));
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(*verticesOut.data()) * verticesOut.size(), verticesOut.data());
		bb.UpdateBoundingGeometryByVertices(verticesOut.data(), verticesOut.size());
	}
	else
	{
		for (int idxSubMesh = 0; idxSubMesh < subMeshCount; ++idxSubMesh) {
			glBeginTransformFeedback(GL_POINTS);
			GLuint renderModeOrg = mesh->GetRenderMode();
			mesh->SetRenderMode(GL_POINTS);
			mesh->DrawSubMesh(idxSubMesh);
			mesh->SetRenderMode(renderModeOrg);
			glEndTransformFeedback();
			glFlush();

			verticesOut.resize(mesh->GetNumVerticesToShader(idxSubMesh));
			glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(*verticesOut.data()) * verticesOut.size(), verticesOut.data());

			BoundingGeometry bbTmp = BoundingGeometry(BoundingGeometryType_AABB);
			bbTmp.UpdateBoundingGeometryByVertices(verticesOut.data(), verticesOut.size());
			bb.SetAABB(bb.GetAABB() | bbTmp.GetAABB());
		}
	}
	
	glDisable(GL_RASTERIZER_DISCARD);

	return bb;
}

void MeshAnimator::InitPreCalcBuffer(shared_ptr<Mesh> mesh)
{
	//TODO: Allow reuse
	if (bufferPreCalc == 0) {
		glGenBuffers(1, &bufferPreCalc);
		glBindBuffer(GL_ARRAY_BUFFER, bufferPreCalc);

		int countSubMesh = mesh->GetSubMeshCount();
		int maxCount = 0;

		if (countSubMesh > 0) {
			for (int i = 0; i < countSubMesh; ++i) {
				maxCount = max(maxCount, (int)mesh->GetNumVerticesToShader(i));
			}
		}
		else
		{
			maxCount = mesh->GetNumVerticesToShader(-1);
		}
		
		glBufferData(GL_ARRAY_BUFFER, maxCount*sizeof(Vector3), nullptr, GL_DYNAMIC_READ);
	}
}
