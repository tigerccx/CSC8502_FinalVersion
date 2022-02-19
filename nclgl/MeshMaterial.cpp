#include "MeshMaterial.h"


using std::ifstream;

MeshMaterial::MeshMaterial(const std::string& filename, bool animated, bool transparent) { //TODO: This is stupid. Do better when you have time
	ifstream file(MESHDIR+filename);

	string dataType;
	file >> dataType;

	if (dataType != "MeshMat") {
		std::cout << "File " << filename << " is not a MeshMaterial!\n";
		return;
	}
	int version;
	file >> version;

	if (version != 1) {
		std::cout << "File " << filename << " has incompatible version " << version << "!\n";
		return;
	}

	int matCount;
	int meshCount;
	file >> matCount;
	file >> meshCount;

	vector<string> names;

	//NOTE: must be saved in the right order!
	for (int i = 0; i < matCount; ++i) {
		string name;
		int count;
		file >> name;
		file >> count;

		shared_ptr<Material> mat = MaterialManager::GetInstance()->CreateMaterial(name);
		if (!mat)
			throw NullPtrException();

		shared_ptr<Shader> shader;

		string fragShaderFile = "texturedFragment.glsl";
		if (transparent) {
			fragShaderFile = "texturedAlphaFragment.glsl";
			mat->renderMode = MaterialRenderMode_Transparent;
		}

		if (animated)
			shader = ShaderManager::GetInstance()->CreateShader("SkinningVertex.glsl", fragShaderFile);
		else
			shader = ShaderManager::GetInstance()->CreateShader("NonSkinningVertex.glsl", fragShaderFile);

		mat->SetShader(shader);

		names.emplace_back(name);

		//TODO: Add more specification to load from file
		for (int j = 0; j < count; ++j) {
			string entryData;
			file >> entryData;
			string channel;
			string file;
			size_t split = entryData.find_first_of(':');
			channel = entryData.substr(0, split);
			file = entryData.substr(split + 1);

			auto texture = make_shared<Texture>(GL_TEXTURE_2D); //TODO: Add more specification to load from file
			if (!texture->Load(string(TEXTUREDIR) + file, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y))
				throw NullPtrException();
			mat->SetTexture(channel, texture);
		}
		materialLayers[name] = mat;
	}

	for (int i = 0; i < meshCount; ++i) {
		int entry;
		file >> entry;
		meshLayers.emplace_back(names[entry]);
	}
}

bool MeshMaterial::AddMeshLayerLink(const string& matName)
{
	if (!Utility::IsKeyInMap(materialLayers, matName))
		return false;
	else
		meshLayers.emplace_back(matName);
}

shared_ptr<Material> MeshMaterial::GetMaterial(const string& name) const
{
	auto itr = materialLayers.find(name);
	if (itr == materialLayers.end()) {
		return nullptr;
	}
	return itr->second;
}

void MeshMaterial::GetMaterials(vector<shared_ptr<Material>>& materialsOut) const
{
	for (auto& itr : materialLayers) {
		materialsOut.emplace_back(itr.second);
	}
}

shared_ptr<Material> MeshMaterial::GetMaterial() const
{
	if (materialLayers.size() == 0) {
		return nullptr;
	}
	return materialLayers.begin()->second;
}

shared_ptr<Material> MeshMaterial::GetMaterialForLayer(int i) const {
	if (i < 0 || i >= (int)meshLayers.size()) {
		return nullptr;
	}
	return GetMaterial(meshLayers[i]);
}