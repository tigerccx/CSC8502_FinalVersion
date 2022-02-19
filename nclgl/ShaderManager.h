#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Singleton.h"
#include "Shader.h"
#include "Utility.h"

using namespace std;

class ShaderManager: public Singleton<ShaderManager>
{
public:
	ShaderManager() = default;
	ShaderManager(const ShaderManager& other) = default;
	ShaderManager& operator=(const ShaderManager& other) = default;
	~ShaderManager();

	shared_ptr<Shader> CreateShader(const string& vertex,
						const string& fragment = "",
						const string& geometry = "", 
						const string& domain = "", 
						const string& hull = "", 
						bool transformFeedback = false, const TransformFeedbackParam& transformFeedbackParam = TransformFeedbackParam());
	bool DeleteShader(const string& name);
	shared_ptr<Shader> FindShader(const string& name);
	inline string GetName(const string& vertex,
		const string& fragment,
		const string& geometry = "",
		const string& domain = "",
		const string& hull = "") {
		return "Vertex::" + vertex + " Fragment::" + fragment + " Geometry::" + geometry + " Domain::" + domain + " Hull::" + hull;
	}

protected:
	map<string, shared_ptr<Shader>> shaderList;
};