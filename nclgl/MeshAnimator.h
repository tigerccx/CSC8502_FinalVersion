#pragma once

#include <vector>
#include <memory>
#include <vector>

#include "Shader.h"
#include "Mesh.h"
#include "MeshAnimation.h"
#include "Utility.h"
#include "BoundingGeometry.h"

#include "ShaderManager.h"

using namespace std;

class MeshAnimator
{
public:
	MeshAnimator() { InitPreCalcShader(); };
	MeshAnimator(shared_ptr<MeshAnimation> meshAnimation) { InitPreCalcShader(); SetMeshAnimation(meshAnimation); }

	float speedScale = 1.0f;

	inline void SetMeshAnimation(shared_ptr<MeshAnimation> meshAnimation) { this->meshAnimation = meshAnimation; }
	inline shared_ptr<MeshAnimation> GetMeshAnimation() const { return meshAnimation; }

	virtual ~MeshAnimator() = default;

	virtual void UpdateFrame(float dt, shared_ptr<Mesh> mesh);
	inline void Pause() { isPaused = true; }
	inline void Resume() { isPaused = false; }
	inline void Reset() {
		currentFrame = 0;
		frameTime = 0;
	}
	inline bool IsPaused() const { return isPaused; }

	void UpdateRenderStateToShader(shared_ptr<Shader> shader);
	void UpdateRenderStateToPreCalcShader() { if (shaderPreCalc) UpdateRenderStateToShader(shaderPreCalc); else throw NullPtrException(); }

	BoundingGeometry CalculateAnimatedMeshBoundingGeometry(shared_ptr<Mesh> mesh);

protected:
	shared_ptr<MeshAnimation> meshAnimation;
	vector<Matrix4> frameMatrices;

	bool isPaused = false;
	int currentFrame = 0;
	float frameTime = 0;

	static shared_ptr<Shader> shaderPreCalc;
	GLuint bufferPreCalc = 0;
	vector<Vector3> verticesOut;

	void InitPreCalcShader();
	void InitPreCalcBuffer(shared_ptr<Mesh> mesh);
};


