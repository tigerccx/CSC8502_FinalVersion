#include "MaterialManager.h"

MaterialManager::~MaterialManager()
{

}

//NOTE: return nullptr if the material already exists. 
shared_ptr<Material> MaterialManager::CreateMaterial(const string& name)
{
	shared_ptr<Material> materialNew = FindMaterial(name);
	if (materialNew) {
		return nullptr; //return nullptr if the material already exists. 
	}

	materialNew = make_shared<Material>();
	materialNew->name = name;

	materialList[name] = materialNew;
	return materialNew;
}

shared_ptr<Material> MaterialManager::FindMaterial(const string& name)
{
	auto itr = materialList.find(name);
	if (itr == materialList.end()) {
		return nullptr;
	}
	else
	{
		return itr->second;
	}
}

bool MaterialManager::DeleteMaterial(const string& name)
{
	auto itr = materialList.find(name);
	if (itr == materialList.end()) {
		return false;
	}
	else
	{
		materialList.erase(itr);
		return true;
	}
}

shared_ptr<Material> MaterialManager::CopyMaterial(const string& nameFrom, const string& name)
{
	auto materialFrom = FindMaterial(nameFrom);
	if (!materialFrom) {
		return nullptr;
	}
	if (!FindMaterial(name)) {
		return nullptr;
	}

	shared_ptr<Material> matNew = make_shared<Material>(*materialFrom);
	matNew->name = name;
	materialList[name] = matNew;
	return matNew;
}

shared_ptr<Material> MaterialManager::CopyMaterial(shared_ptr<Material> materialFrom, const string& name)
{
	if(!FindMaterial(name)) {
		return nullptr;
	}
	shared_ptr<Material> matNew = make_shared<Material>(*materialFrom);
	matNew->name = name;
	materialList[name] = matNew;
	return matNew;
}
