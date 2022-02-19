#pragma once

#include <string>
#include <vector>
#include <map>
#include <array>
#include <memory>

#include "KHR\khrplatform.h"
#include "glad\glad.h"

#include "GL/GL.h"
#include "KHR/WGLext.h"

#include "SOIL/SOIL.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"

#include "Shader.h"
#include "Texture.h"
#include "Utility.h"

enum MaterialRenderMode {
	MaterialRenderMode_Common = 0,
	MaterialRenderMode_Transparent = 1000,
};

template<class T>
class MaterialVariableArray {
public:
	MaterialVariableArray(vector<T>* data) : data(data) {}
	~MaterialVariableArray() { if (data) delete data; }
	vector<T>* data;
	int length;
};

class MaterialTexture {
public:
	MaterialTexture() = default;
	~MaterialTexture() = default;
	MaterialTexture(const MaterialTexture& other) = default;
	MaterialTexture& operator=(const MaterialTexture& other) = default;
	shared_ptr<Texture> texture = nullptr;
	float textureScale = 1;
	int useTexture = 1;
	Matrix4 textureMatrix;
};

class Material
{
public:
	Material();
	Material(const Material& mat) = default;
	Material& operator=(const Material& mat) = default;
	virtual ~Material();

	MaterialRenderMode renderMode = MaterialRenderMode_Common;
	string name = "Material";

	inline void SetShader(shared_ptr<Shader> shader) {
		this->shader = shader;
	}
	inline shared_ptr<Shader> GetShader() { return shader; }
	inline void BindShader() { glUseProgram(shader->GetProgram()); }

	void UpdateData() {
		UploadUniform1f();
		UploadUniform1i();
		UploadUniform1ui();
		UploadTexture();

		UploadUniform1fv();
		UploadUniform2fv();
		UploadUniform3fv();
		UploadUniform4fv();

		UploadUniformMatrix2fv();
		UploadUniformMatrix3fv();
		UploadUniformMatrix4fv();
	}

	void UnbindData() {
		UnbindTexture();
	}

	//
	//Single variable
	//
	inline void SetFloat(const string& name, float f) { map_1f[name] = f; }
	inline bool GetFloat(const string& name, float* f) { auto itr = map_1f.find(name); if (itr != map_1f.end()) { *f = itr->second; return true; } else return false; }
	inline void SetInt(const string& name, int i) { map_1i[name] = i; }
	inline bool GetInt(const string& name, int* i) { auto itr = map_1i.find(name); if (itr != map_1i.end()) { *i = itr->second; return true; } else return false; }
	inline void SetUInt(const string& name, unsigned int ui) { map_1ui[name] = ui; }
	inline bool GetUInt(const string& name, unsigned int* ui) { auto itr = map_1ui.find(name); if (itr != map_1ui.end()) { *ui = itr->second; return true; } else return false; }
	inline void SetTexture(const string& name, shared_ptr<Texture> tex) { MaterialTexture mt; mt.texture = tex; map_texture[name] = mt; }
	inline bool SetTextureMatrix(const string& name, Matrix4 mat) { auto itr = map_texture.find(name); if (itr != map_texture.end()) { itr->second.textureMatrix = mat; return true; } else return false; }
	inline bool SetTextureScale(const string& name, float scale) { auto itr = map_texture.find(name); if (itr != map_texture.end()) { itr->second.textureScale = scale; return true; } else return false; }
	inline bool GetTexture(const string& name, shared_ptr<Texture> tex) { auto itr = map_texture.find(name); if (itr != map_texture.end()) { tex = itr->second.texture; return true; } else return false; }
	inline bool GetTextureMatrix(const string& name, Matrix4* mat) { auto itr = map_texture.find(name); if (itr != map_texture.end()) { *mat = itr->second.textureMatrix; return true; } else return false; }
	inline bool GetTextureScale(const string& name, float* scale) { auto itr = map_texture.find(name); if (itr != map_texture.end()) { *scale = itr->second.textureScale; return true; } else return false; }

	//
	//Array of float vectors
	//
	inline void Set1FloatVectors(const string& name, vector<float>* fv) {
		if (Utility::IsKeyInMap(map_1fv, name))
			delete map_1fv[name];
		map_1fv[name] = new MaterialVariableArray<float>(fv);
	}
	void Set2FloatVectors(const string& name, vector<Vector2>* fv) {
		vector<float>* fv_ = new vector<float>();
		for (int i = 0; i < fv->size(); ++i) {
			fv_->emplace_back((*fv)[i].x);
			fv_->emplace_back((*fv)[i].y);
		}
		if (Utility::IsKeyInMap(map_2fv, name))
			delete map_2fv[name];
		map_2fv[name] = new MaterialVariableArray<float>(fv_);
	}
	void Set2FloatVector(const string& name, Vector2 fv) {
		auto tmp = vector<Vector2>({ fv });
		Set2FloatVectors(name, &tmp);
	}
	void Set3FloatVectors(const string& name, vector<Vector3>* fv) {
		vector<float>* fv_ = new vector<float>();
		for (int i = 0; i < fv->size(); ++i) {
			fv_->emplace_back((*fv)[i].x);
			fv_->emplace_back((*fv)[i].y);
			fv_->emplace_back((*fv)[i].z);
		}
		if (Utility::IsKeyInMap(map_3fv, name))
			delete map_3fv[name];
		map_3fv[name] = new MaterialVariableArray<float>(fv_);
	}
	void Set3FloatVector(const string& name, Vector3 fv) {
		auto tmp = vector<Vector3>({ fv });
		Set3FloatVectors(name, &tmp);
	}
	void Set4FloatVectors(const string& name, vector<Vector4>* fv) {
		vector<float>* fv_ = new vector<float>();
		for (int i = 0; i < fv->size(); ++i) {
			fv_->emplace_back((*fv)[i].x);
			fv_->emplace_back((*fv)[i].y);
			fv_->emplace_back((*fv)[i].z);
			fv_->emplace_back((*fv)[i].w);
		}
		if (Utility::IsKeyInMap(map_4fv, name))
			delete map_4fv[name];
		map_4fv[name] = new MaterialVariableArray<float>(fv_);
	}
	void Set4FloatVector(const string& name, Vector4 fv) {
		auto tmp = vector<Vector4>({ fv });
		Set4FloatVectors(name, &tmp);
	}

	//
	//Array of square matrix
	//
	void SetMatrix2s(const string& name, vector<Matrix2>* fv) {
		vector<float>* fv_ = new vector<float>();
		for (int i = 0; i < fv->size(); ++i) {
			for (int j = 0; j < 4; ++j) {
				fv_->emplace_back((*fv)[i].values[j]);
			}
		}
		if (Utility::IsKeyInMap(map_matrix2fv, name))
			delete map_matrix2fv[name];
		map_matrix2fv[name] = new MaterialVariableArray<float>(fv_);
	}
	void SetMatrix2(const string& name, Matrix2 fv) {
		auto tmp = vector<Matrix2>({ fv });
		SetMatrix2s(name, &tmp);
	}
	void SetMatrix3s(const string& name, vector<Matrix3>* fv) {
		vector<float>* fv_ = new vector<float>();
		for (int i = 0; i < fv->size(); ++i) {
			for (int j = 0; j < 9; ++j) {
				fv_->emplace_back((*fv)[i].values[j]);
			}
		}
		if (Utility::IsKeyInMap(map_matrix3fv, name))
			delete map_matrix3fv[name];
		map_matrix3fv[name] = new MaterialVariableArray<float>(fv_);
	}
	void SetMatrix3(const string& name, Matrix3 fv) {
		auto tmp = vector<Matrix3>({ fv });
		SetMatrix3s(name, &tmp);
	}
	void SetMatrix4s(const string& name, vector<Matrix4>* fv) {
		vector<float>* fv_ = new vector<float>();
		for (int i = 0; i < fv->size(); ++i) {
			for (int j = 0; j < 16; ++j) {
				fv_->emplace_back((*fv)[i].values[j]);
			}
		}
		if (Utility::IsKeyInMap(map_matrix4fv, name))
			delete map_matrix4fv[name];
		map_matrix4fv[name] = new MaterialVariableArray<float>(fv_);
	}
	void SetMatrix4(const string& name, Matrix4 fv) {
		auto tmp = vector<Matrix4>({ fv });
		SetMatrix4s(name, &tmp);
	}

protected:
	shared_ptr<Shader> shader;

	//
	//Single variable
	//
	map<string, float> map_1f;
	void UploadUniform1f() {
		for (const auto& itr : map_1f) {
			glUniform1f(glGetUniformLocation(shader->GetProgram(), itr.first.c_str()), itr.second);
		}
	}
	map<string, int> map_1i;
	void UploadUniform1i() {
		for (const auto& itr : map_1i) {
			glUniform1i(glGetUniformLocation(shader->GetProgram(), itr.first.c_str()), itr.second);
		}
	}
	map<string, unsigned int> map_1ui;
	void UploadUniform1ui() {
		for (const auto& itr : map_1ui) {
			glUniform1ui(glGetUniformLocation(shader->GetProgram(), itr.first.c_str()), itr.second);
		}
	}
	map<string, MaterialTexture> map_texture;
	void UploadTexture() {
		int idOffset = 0;

		for (const auto& itr : map_texture) {
			itr.second.texture->BindTexture(shader, itr.first, idOffset);
			glUniform1i(glGetUniformLocation(shader->GetProgram(), (itr.first + string(NCLGL_SHADER_TEXTURE_TOUSE_APPENDIX)).c_str()), itr.second.useTexture);
			glUniform1f(glGetUniformLocation(shader->GetProgram(), (itr.first + string(NCLGL_SHADER_TEXTURE_SCALE_APPENDIX)).c_str()), itr.second.textureScale);
			glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), (itr.first + string(NCLGL_SHADER_TEXTURE_MATRIX_APPENDIX)).c_str()), 1, false, itr.second.textureMatrix.values);
			idOffset += 1;
		}
	}

	void UnbindTexture() {
		int idOffset = 0;
		for (const auto& itr : map_texture) {
			itr.second.texture->BindTexture(shader, itr.first, idOffset);
			glUniform1i(glGetUniformLocation(shader->GetProgram(), (itr.first + string(NCLGL_SHADER_TEXTURE_TOUSE_APPENDIX)).c_str()), 0);
			glUniform1f(glGetUniformLocation(shader->GetProgram(), (itr.first + string(NCLGL_SHADER_TEXTURE_SCALE_APPENDIX)).c_str()), 1);
			glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), (itr.first + string(NCLGL_SHADER_TEXTURE_MATRIX_APPENDIX)).c_str()), 1, false, Matrix4::identity.values);
			itr.second.texture->UnbindTexture(shader, idOffset);
		}
	}

	//
	//Array of float vectors
	//
	map<string, MaterialVariableArray<float>*> map_1fv;
	void UploadUniform1fv() {
		for (const auto& itr : map_1fv) {
			auto* data = (itr.second)->data;
			glUniform1fv(glGetUniformLocation(shader->GetProgram(), itr.first.c_str()), data->size(), data->data());
		}
	}
	map<string, MaterialVariableArray<float>*> map_2fv;
	void UploadUniform2fv() {
		for (const auto& itr : map_2fv) {
			auto* data = (itr.second)->data;
			glUniform2fv(glGetUniformLocation(shader->GetProgram(), itr.first.c_str()), data->size() / 2, data->data());
		}
	}
	map<string, MaterialVariableArray<float>*> map_3fv;
	void UploadUniform3fv() {
		for (const auto& itr : map_3fv) {
			auto* data = (itr.second)->data;
			glUniform3fv(glGetUniformLocation(shader->GetProgram(), itr.first.c_str()), data->size() / 3, data->data());
		}
	}
	map<string, MaterialVariableArray<float>*> map_4fv;
	void UploadUniform4fv() {
		for (const auto& itr : map_4fv) {
			auto* data = (itr.second)->data;
			glUniform4fv(glGetUniformLocation(shader->GetProgram(), itr.first.c_str()), data->size() / 4, data->data());
		}
	}

	//
	//Array of square matrix
	//
	map<string, MaterialVariableArray<float>*> map_matrix2fv;
	void UploadUniformMatrix2fv() {
		for (const auto& itr : map_matrix2fv) {
			auto* data = (itr.second)->data;
			glUniformMatrix2fv(glGetUniformLocation(shader->GetProgram(), itr.first.c_str()), data->size() / 4, false, data->data());
		}
	}
	map<string, MaterialVariableArray<float>*> map_matrix3fv;
	void UploadUniformMatrix3fv() {
		for (const auto& itr : map_matrix3fv) {
			auto* data = (itr.second)->data;
			glUniformMatrix3fv(glGetUniformLocation(shader->GetProgram(), itr.first.c_str()), data->size() / 9, false, data->data());
		}
	}
	map<string, MaterialVariableArray<float>*> map_matrix4fv;
	void UploadUniformMatrix4fv() {
		for (const auto& itr : map_matrix4fv) {
			auto* data = (itr.second)->data;
			glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), itr.first.c_str()), data->size() / 16, false, data->data());
		}
	}
};

