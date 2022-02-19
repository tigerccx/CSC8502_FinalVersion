#include "Material.h"

Material::Material():
	shader(nullptr)
{
}

Material::~Material()
{
	for (const auto& itr : map_1fv) { if (itr.second) delete itr.second; }
	for (const auto& itr : map_2fv) { if (itr.second) delete itr.second; }
	for (const auto& itr : map_3fv) { if (itr.second) delete itr.second; }
	for (const auto& itr : map_4fv) { if (itr.second) delete itr.second; }
	for (const auto& itr : map_matrix2fv) { if (itr.second) delete itr.second; }
	for (const auto& itr : map_matrix3fv) { if (itr.second) delete itr.second; }
	for (const auto& itr : map_matrix4fv) { if (itr.second) delete itr.second; }
}
