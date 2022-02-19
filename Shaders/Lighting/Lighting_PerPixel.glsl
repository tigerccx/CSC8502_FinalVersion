//Lighting
const int NCLGL_LIGHT_MAX_PERPIXEL = 8;

uniform int lightCount;
uniform int lights[NCLGL_LIGHT_MAX_PERPIXEL];
uniform vec4 lightColours[NCLGL_LIGHT_MAX_PERPIXEL];
uniform vec3 lightPositions[NCLGL_LIGHT_MAX_PERPIXEL];
uniform vec4 lightParams[NCLGL_LIGHT_MAX_PERPIXEL];

const float thresAtRadius = 0.0001;
const vec4 ambientColour = vec4(1,1,1,0);
const float PI = 3.14159265359;

float GetAttenuationDirectionalLight(vec4 lightColour){
	return lightColour.a;
}

float GetAttenuationPointLight(vec3 worldPos, vec3 lightPos, vec4 lightColour, vec4 lightParam){
	float distance = length(lightPos - worldPos);
	float lightRadius = lightParam.x;
	return (clamp((lightRadius * lightRadius) / (distance * distance) * thresAtRadius - thresAtRadius, 0.0f, 1.0f - thresAtRadius)) * lightColour.a;
}

float GetAttenuationSpotLight(vec3 worldPos, vec3 lightPos, vec4 lightColour, vec4 lightParam){
	float dirX = lightParam.x;
	float dirY = lightParam.y;
	float dirZ =  sign(lightParam.z)*sqrt(abs(1-dirX*dirX-dirY*dirY));
	vec3 dirLight = vec3(dirX, dirY, dirZ);
	
	float cosHalfAngle = lightParam.w;
	
	vec3 toLight = normalize(lightPos - worldPos);
	float cosFragment = dot(dirLight, -toLight);
	if(cosFragment>cosHalfAngle) //If in spot light area
	{
		float lightRadius = abs(lightParam.z);
		float distance = length(lightPos - worldPos);
		float attenuation = (clamp((lightRadius * lightRadius) / (distance * distance) * thresAtRadius - thresAtRadius, 0.0f, 1.0f - thresAtRadius)) * lightColour.a;
		float sqrSinFragment = abs(1-cosFragment*cosFragment);
		float sqrSinHalfAngle = abs(1-cosHalfAngle*cosHalfAngle);
		float scaleAttenu = (sqrSinHalfAngle-sqrSinFragment)/sqrSinHalfAngle;
		return attenuation *  scaleAttenu * scaleAttenu;
	}
	else{
		return 0.0f;
	}
}

void SurfaceCommon(vec3 toLight, vec3 toEye,
					vec3 normal, 
					vec3 diffuseColour, vec3 lightColour, 
					float specGloss, float attenuation, 
					out vec3 diffuse, out vec3 specular){
	vec3 halfDir = normalize(toLight + toEye);
	float lambert = max(dot(toLight, normal), 0.0f);
	
	float specFactor = clamp(dot(halfDir, normal), 0.0f, 1.0f);
	specFactor = pow(specFactor, specGloss);
	vec3 surface = diffuseColour * lightColour;
	diffuse = surface * lambert * attenuation;
	specular = (lightColour * specFactor) * attenuation * 0.33; //0.33 is optional
}

void CalculateDirectionalLight( vec3 toEye, vec3 normal,
								vec3 diffuseColour, float specGloss,
								vec4 lightColour, vec3 lightPos, vec4 lightParam, 
								out vec3 diffuse, out vec3 specular){
	vec3 toLight = -vec3(lightParam.x, lightParam.y, lightParam.z);
	float attenuation = lightColour.a;
	
	SurfaceCommon(toLight, toEye, normal, 
					diffuseColour, lightColour.rgb, 
					specGloss, attenuation, diffuse, specular);
}

void CalculatePointLight( vec3 toEye, vec3 worldPos, vec3 normal,
							vec3 diffuseColour, float specGloss, 
							vec4 lightColour, vec3 lightPos, vec4 lightParam, 
							out vec3 diffuse, out vec3 specular){
	float lightRadius = lightParam.x;
	vec3 toLight = normalize(lightPos - worldPos);
	float distance = length(lightPos - worldPos);
	float attenuation = (clamp((lightRadius * lightRadius) / (distance * distance) * thresAtRadius - thresAtRadius, 0.0f, 1.0f - thresAtRadius)) * lightColour.a;

	SurfaceCommon(toLight, toEye, normal, 
					diffuseColour, lightColour.rgb, 
					specGloss, attenuation, diffuse, specular);
}

void CalculateSpotLight( vec3 toEye, vec3 worldPos, vec3 normal,
							vec3 diffuseColour, float specGloss,
							vec4 lightColour, vec3 lightPos, vec4 lightParam, 
							out vec3 diffuse, out vec3 specular){
	float dirX = lightParam.x;
	float dirY = lightParam.y;
	float dirZ =  sign(lightParam.z)*sqrt(abs(1-dirX*dirX-dirY*dirY));
	vec3 dirLight = vec3(dirX, dirY, dirZ);
	
	float lightRadius = abs(lightParam.z);
	float cosHalfAngle = lightParam.w;
	
	vec3 toLight = normalize(lightPos - worldPos);
	float cosFragment = dot(dirLight, -toLight);
	if(cosFragment>cosHalfAngle) //If in spot light area
	{
		vec3 halfDir = normalize(toLight + toEye);
		float distance = length(lightPos - worldPos);
		float attenuation = (clamp((lightRadius * lightRadius) / (distance * distance) * thresAtRadius - thresAtRadius, 0.0f, 1.0f - thresAtRadius)) * lightColour.a;
		float sqrSinFragment = abs(1-cosFragment*cosFragment);
		float sqrSinHalfAngle = abs(1-cosHalfAngle*cosHalfAngle);
		float scaleAttenu = (sqrSinHalfAngle-sqrSinFragment)/sqrSinHalfAngle;
		attenuation = attenuation *  scaleAttenu * scaleAttenu;

		SurfaceCommon(toLight, toEye, normal, 
					diffuseColour, lightColour.rgb, 
					specGloss, attenuation, diffuse, specular);
	}
	else{
		diffuse = vec3(0,0,0);
		specular = vec3(0,0,0);
	}
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

void SurfacePBR(vec3 toLight, vec3 toEye,
					vec3 normal, 
					vec3 albedo, vec3 lightColour, 
					float metallic, float roughness, float attenuation, 
					out vec3 clr){
	vec3 halfDir = normalize(toLight + toEye);
					
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	vec3 F = fresnelSchlick(max(dot(halfDir, toLight), 0.0), F0);
	
	float NDF = DistributionGGX(normal, halfDir, roughness);
	float G   = GeometrySmith(normal, toEye, toLight, roughness);      
	
	vec3 numerator    = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, toEye), 0.0) * max(dot(normal, toEye), 0.0)  + 0.0001;
	vec3 specular     = numerator / denominator;  
	
	vec3 kS = F;
	vec3 kD  = vec3(1.0) - kS;
	kD  *= 1.0 - metallic;
	
	float NdotL = max(dot(normal, toLight), 0.0);
	vec3 radiance = lightColour.rgb*attenuation;
	clr = clamp((kD *albedo/PI+specular)*radiance*NdotL, vec3(0,0,0), lightColour.rgb);
}

void CalculateDirectionalLightPBR( vec3 toEye, vec3 worldPos, vec3 normal,
								vec3 albedo, 
								float metallic, float roughness,  
								vec4 lightColour, vec3 lightPos, vec4 lightParam, 
								out vec3 clr){
	vec3 toLight = -vec3(lightParam.x, lightParam.y, lightParam.z);
	float attenuation = clamp(lightColour.a, 0.0f, 1.0f);
	
	SurfacePBR(toLight, toEye, normal, 
				albedo, lightColour.rgb, 
				metallic, roughness, attenuation, 
				clr);
}

void CalculatePointLightPBR( vec3 toEye, vec3 worldPos, vec3 normal,
							vec3 albedo, 
							float metallic, float roughness,  
							vec4 lightColour, vec3 lightPos, vec4 lightParam, 
							out vec3 clr){
	float lightRadius = lightParam.x;
	vec3 toLight = normalize(lightPos - worldPos);
	float distance = length(lightPos - worldPos);
	float attenuation = (clamp(((lightRadius * lightRadius) / (distance * distance) * thresAtRadius - thresAtRadius)* lightColour.a, 0.0f, 1.0f - thresAtRadius));
	
	SurfacePBR(toLight, toEye, normal, 
				albedo, lightColour.rgb, 
				metallic, roughness, attenuation, 
				clr);
}

void CalculateSpotLightPBR( vec3 toEye, vec3 worldPos, vec3 normal,
							vec3 albedo, 
							float metallic, float roughness,  
							vec4 lightColour, vec3 lightPos, vec4 lightParam, 
							out vec3 clr){
	float dirX = lightParam.x;
	float dirY = lightParam.y;
	float dirZ =  sign(lightParam.z)*sqrt(abs(1-dirX*dirX-dirY*dirY));
	vec3 dirLight = vec3(dirX, dirY, dirZ);
	
	float lightRadius = abs(lightParam.z);
	float cosHalfAngle = lightParam.w;
	
	vec3 toLight = normalize(lightPos - worldPos);
	float cosFragment = dot(dirLight, -toLight);
	if(cosFragment>cosHalfAngle) //If in spot light area
	{
		vec3 halfDir = normalize(toLight + toEye);
		float distance = length(lightPos - worldPos);
		float attenuation = (clamp(((lightRadius * lightRadius) / (distance * distance) * thresAtRadius - thresAtRadius)* lightColour.a, 0.0f, 1.0f - thresAtRadius));
		float sqrSinFragment = abs(1-cosFragment*cosFragment);
		float sqrSinHalfAngle = abs(1-cosHalfAngle*cosHalfAngle);
		float scaleAttenu = (sqrSinHalfAngle-sqrSinFragment)/sqrSinHalfAngle;
		attenuation = attenuation *  scaleAttenu * scaleAttenu;

		SurfacePBR(toLight, toEye, normal, 
				albedo, lightColour.rgb, 
				metallic, roughness, attenuation, 
				clr);
	}
	else{
		clr = vec3(0,0,0);
	}
}