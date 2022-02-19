#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <algorithm> 

#include "Material.h"
#include "MaterialManager.h"
#include "ShaderManager.h"

#include <fstream>
#include <iostream>

#include "common.h"


using std::map;
using std::string;
using std::vector;

class MeshMaterial
{
public:
	MeshMaterial() = default;
	MeshMaterial(const string& filename, bool animated = false, bool transparent=false);
	virtual ~MeshMaterial() {}

	//TODO: make meshLayers link to material by name to name instead of whatever this is
	inline void AddMaterial(shared_ptr<Material> mat) { materialLayers[mat->name] = mat; }
	bool AddMeshLayerLink(const string& matName); 
	void AddMaterialForSubMesh(int idx, shared_ptr<Material> mat) {
		AddMaterial(mat);
		if (meshLayers.size() <= idx) {
			meshLayers.resize(idx + 1);
		}
		meshLayers[idx] = mat->name;
	}
	void AddMaterialsForSubMeshes(const vector<shared_ptr<Material>>& mats) {
		for (auto& mat : mats) {
			AddMaterial(mat);
			AddMeshLayerLink(mat->name);
		}
	}

	inline bool CheckSupportMeshLayers() const { return meshLayers.size() > 0; }
	inline int GetMaterialCount() const { return materialLayers.size(); }
	shared_ptr<Material> GetMaterial(const string& name) const;
	void GetMaterials(vector<shared_ptr<Material>>& materialsOut) const;
	shared_ptr<Material> GetMaterial() const;
	shared_ptr<Material> GetMaterialForLayer(int i) const;

protected:
	map<string, shared_ptr<Material>> materialLayers; //Material name -> material
	vector<string>	meshLayers; // submesh[i] <-> meshLayers[i]
};

