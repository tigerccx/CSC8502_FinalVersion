#include "LightManager.h"

LightManager::~LightManager()
{
	while (!lightList.empty()) {
		Light* light = lightList[lightList.size() - 1];
		if (light)
			delete light;
		lightList.pop_back();
	}
}

bool LightManager::DeleteLight(Light* camNew)
{
	auto itr = find(lightList.begin(), lightList.end(), camNew);
	if (itr == lightList.end()) {
		return false;
	}
	else
	{
		delete* itr;
		itr = lightList.erase(itr);
		return true;
	}
}

void LightManager::UploadLightsDataToShader(shared_ptr<Shader> shader)
{
	//TODO: calculate if object is in light ... 
	//for first NCLGL_LIGHT_MAX_PERPIXEL lights: do perpixel shading
	//for following NCLGL_LIGHT_MAX_PERVERTEX lights: do pervertex shading\

	glUseProgram(shader->GetProgram());

	vector<int> lights;
	vector<Vector4> lightColours;
	vector<Vector3> lightPositions;
	vector<Vector4> lightParams;

	int countValid = 0;
	for (int idxLight = 0; idxLight < lightList.size(); ++idxLight) {
		Light* light = lightList[idxLight];
		if (!light->IsActive())
			continue;

		auto goLight = light->GetGO();
		if (!goLight)
			throw NullPtrException();
		auto trans = light->GetGO()->transform;

		lights.emplace_back((int)light->GetLightType());
		lightColours.emplace_back(light->GetLightColourAndIntensity());
		lightPositions.emplace_back(trans->GetPositionLocal());
		lightParams.emplace_back(light->GetLightParameterPackedVector());
		countValid += 1;
	}
	int lightCount = min(NCLGL_LIGHT_MAX_PERPIXEL, countValid);

	glUniform1i(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_LIGHT_COUNT), lightCount);
	glUniform1iv(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_LIGHT_TYPE), 
		lightList.size(), (int*)lights.data());
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_LIGHT_COLOUR),
		lightColours.size(), (float*)lightColours.data());
	glUniform3fv(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_LIGHT_POSITIONS),
		lightPositions.size(), (float*)lightPositions.data());
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_LIGHT_PARAMETERS),
		lightParams.size(), (float*)lightParams.data());
}

void LightManager::UploadShadowDataToShader(shared_ptr<Shader> shader)
{
	vector<int> castShaodw;
	castShaodw.resize(8);
	vector<float> shadowMatrices;
	vector<shared_ptr<Texture>> shadowMaps;
	shadowMaps.resize(NCLGL_LIGHT_MAX_PERPIXEL);
	vector<shared_ptr<Texture>> shadowCubeMaps;
	shadowCubeMaps.resize(NCLGL_LIGHT_MAX_PERPIXEL);
	for (int idxLight = 0; idxLight < lightList.size(); ++idxLight) {
		//TODO: Add support for point light
		Light* light = lightList[idxLight];
		if (!light->IsActive())
			continue;

		if (light->GetCastShadow()) {
			castShaodw[idxLight] = 1;
			switch (light->GetLightType())
			{
			case LightType_Directional: {
				auto shadowMatrix = light->GetShadowMatrix();
				for (int i = 0; i < 16; ++i) {
					shadowMatrices.emplace_back(shadowMatrix[i]);
				}
				for (int iMatrix = 1; iMatrix < 6; ++iMatrix) {
					for (int i = 0; i < 16; ++i) {
						shadowMatrices.emplace_back(Matrix4::identity.values[i]);
					}
				}
				shadowMaps[idxLight] = (light->GetShadowMap()->GetDepthTexture());
				shadowCubeMaps[idxLight] = (nullptr);
			}
				
				break;
			case LightType_Point: {
				for (int iMatrix = 0; iMatrix < 6; ++iMatrix) {
					auto shadowMatrix = light->GetShadowMatrix(iMatrix);
					for (int i = 0; i < 16; ++i) {
						shadowMatrices.emplace_back(shadowMatrix[i]);
					}
				}
				shadowCubeMaps[idxLight] = (light->GetShadowCubeMap()->GetDepthTexture());
				shadowMaps[idxLight] = (nullptr);
			}
				break;
			case LightType_Spot: { //TODO: Too many matrices uploaded!!!! Optimise efficiency!
				auto shadowMatrix = light->GetShadowMatrix();
				for (int i = 0; i < 16; ++i) {
					shadowMatrices.emplace_back(shadowMatrix[i]);
				}
				for (int iMatrix = 1; iMatrix < 6; ++iMatrix) {
					for (int i = 0; i < 16; ++i) {
						shadowMatrices.emplace_back(Matrix4::identity.values[i]);
					}
				}
				shadowMaps[idxLight] = (light->GetShadowMap()->GetDepthTexture());
				shadowCubeMaps[idxLight] = (nullptr);
			}
				break;
			default:
				break;
			}
		}
		else {
			for (int iMatrix = 0; iMatrix < 6; ++iMatrix) {
				for (int i = 0; i < 16; ++i) {
					shadowMatrices.emplace_back(Matrix4::identity.values[i]);
				}
			}
			shadowCubeMaps[idxLight] = (nullptr);
			shadowMaps[idxLight] = (nullptr);
			castShaodw[idxLight] = 0;
		}
	}
	glUniform1iv(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_LIGHT_SHADOW_TO_CAST), castShaodw.size(), castShaodw.data());
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_LIGHT_SHADOW_MATRICES), 
		shadowMatrices.size() / 16, false, shadowMatrices.data());
	Texture::BindTextures(shader, shadowMaps, NCLGL_SHADER_LIGHT_SHADOW_MAPS, NCLGL_TEXTURESLOT_SHADOWMAPS_START);
	//TODO: Use smaplerCubemapArray to upload multiple cubemaps
	Texture::BindTextures(shader, shadowCubeMaps, NCLGL_SHADER_LIGHT_SHADOW_CUBEMAPS, NCLGL_TEXTURESLOT_SHADOWCUBEMAPS_START);
	/*if(shadowCubeMaps[0])
		shadowCubeMaps[0]->BindTexture(shader, NCLGL_SHADER_LIGHT_SHADOW_CUBEMAPS, NCLGL_TEXTURESLOT_SHADOWCUBEMAPS_START);*/
}
