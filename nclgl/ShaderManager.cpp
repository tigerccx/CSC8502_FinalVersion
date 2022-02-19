#include "ShaderManager.h"

ShaderManager::~ShaderManager()
{
	
}

shared_ptr<Shader> ShaderManager::CreateShader(const string& vertex,
	const string& fragment, 
	const string& geometry, 
	const string& domain, 
	const string& hull, 
	bool transformFeedback, const TransformFeedbackParam& transformFeedbackParam)
{
	string name = GetName(vertex, fragment, geometry, domain, hull);
	shared_ptr<Shader> shaderNew = FindShader(name);
	if (shaderNew) {
		return shaderNew;
	}

	shaderNew = make_shared<Shader>(vertex, fragment, geometry, domain, hull, 
		transformFeedback, transformFeedbackParam);
	if (!shaderNew->LoadSuccess())
		return nullptr;

	shaderList[name] = shaderNew;
	return shaderNew;
}

bool ShaderManager::DeleteShader(const string& name)
{
	auto itr = shaderList.find(name);
	if (itr == shaderList.end()) {
		return false;
	}
	else
	{
		shaderList.erase(itr);
		return true;
	}
}

shared_ptr<Shader> ShaderManager::FindShader(const string& name)
{
	auto itr = shaderList.find(name);
	if (itr == shaderList.end()) {
		return nullptr;
	}
	else
	{
		return itr->second;
	}
}
