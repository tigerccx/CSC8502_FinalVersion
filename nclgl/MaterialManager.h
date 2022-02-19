#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

#include "Singleton.h"
#include "Material.h"
#include "Utility.h"

using namespace std;

class MaterialManager: public Singleton<MaterialManager>
{
public:
	MaterialManager() = default;
	MaterialManager(const MaterialManager& other) = default;
	MaterialManager& operator=(const MaterialManager& other) = default;
	~MaterialManager();

	shared_ptr<Material> CreateMaterial(const string& name);
	shared_ptr<Material> FindMaterial(const string& name);
	bool DeleteMaterial(const string& name);
	shared_ptr<Material> CopyMaterial(const string& nameFrom, const string& name);
	shared_ptr<Material> CopyMaterial(shared_ptr<Material> materialFrom, const string& name);

protected:
	map<string, shared_ptr<Material>> materialList;
};

