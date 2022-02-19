#version 330 core
#include "Lighting/Lighting_PerPixel.glsl"

//Shadow
uniform int castShadow[NCLGL_LIGHT_MAX_PERPIXEL];
uniform sampler2D shadowTexs[NCLGL_LIGHT_MAX_PERPIXEL];
uniform samplerCube shadowCubeTexs[NCLGL_LIGHT_MAX_PERPIXEL];
//uniform samplerCube shadowCubeTexs;
uniform mat4 shadowMatrices[NCLGL_LIGHT_MAX_PERPIXEL*6];

uniform sampler2D screenTex;
uniform int steps=500;
uniform float stepLength=0.2f;
uniform float strength = 0.01f;

uniform vec3 cameraPos;

const float _HGFactor = 0.0f;

in Vertex{
	vec2 texCoord;
	vec3 dirRay;
} IN;

out vec4 fragColor;

float phaseHG(vec3 lightDir, vec3 viewDir)
{
	float g = _HGFactor;
	return (1 - g * g) / (4 * PI * pow(1 + g * g - 2 * g * dot(viewDir, lightDir), 1.5)); 
}

void main() {

	vec3 dirStep = normalize(IN.dirRay);
	vec3 toEye = -dirStep;
	
	vec3 clr = vec3(0,0,0);
	
	for (int i = 0; i < lightCount; ++i){
		int lightType = lights[i];
		vec3 lightPos = lightPositions[i];
		vec4 lightParam = lightParams[i];
		vec4 lightColour = lightColours[i];
		int toCastShadow = castShadow[i];
		float shadow = 1.0f;
		mat4 shadowMatrix;
		
		vec3 dirLight = vec3(0,0,0);
		if(lightType == 2){
			float dirX = lightParam.x;
			float dirY = lightParam.y;
			float dirZ =  sign(lightParam.z)*sqrt(abs(1-dirX*dirX-dirY*dirY));
			dirLight = vec3(dirX, dirY, dirZ);
		}
		
		vec3 posStep = IN.dirRay+cameraPos;
		float stepLen = stepLength;
		float transmittance = exp(-stepLength*1.0f);
		vec3 clrLight = vec3(0,0,0);
		
		float testAt = 0.0f;
		
		for(int step=0;step<steps;++step){
			vec3 toLight = normalize(lightPos - posStep);
			vec3 world2Light = lightPos-posStep;
			
			//Calc attenuation and shadow
			float attenuation = 0.0f;
			if(lightType == 0){ //Directional Light
				attenuation = lightColour.a;
				if(toCastShadow>0){
					//Shadow
					shadowMatrix = shadowMatrices[i*6];
					vec4 shadowProj = shadowMatrix*(vec4(posStep,1));
					vec3 shadowNDC = shadowProj.xyz;
					if(abs(shadowNDC.x)<1.0f&&abs(shadowNDC.y)<1.0f&&abs(shadowNDC.z)<1.0f){
						vec3 biasCoord = shadowNDC*0.5f + 0.5f;
						float shadowZ = texture(shadowTexs[i], biasCoord.xy).x;
						if(shadowZ<(biasCoord.z)){ //Inside Shadow  
							shadow = 0.0f;
						}
					}
				}
			}
			else if (lightType == 1) { //Point Light
				float dis = length(lightPos - posStep);
				float lightRadius = lightParam.x;
				attenuation = (clamp((lightRadius * lightRadius) / (dis * dis) * thresAtRadius - thresAtRadius, 0.0f, 1.0f - thresAtRadius)) * lightColour.a;
				if(toCastShadow>0){
					//Shadow
					float biasZ = distance(posStep, lightPositions[i])/lightParams[i].x;
					vec3 light2World = world2Light;
					light2World.z = -light2World.z;
					float shadowZ = texture(shadowCubeTexs[i], light2World).r;
					if(shadowZ<biasZ){ //Inside Shadow
						shadow = 0.0f;
					}
				}
			}
			else if (lightType == 2) { //Spot Light
				float cosHalfAngle = lightParam.w;
				
				float cosFragment = dot(dirLight, -toLight);
				if(cosFragment>cosHalfAngle) //If in spot light area
				{
					float lightRadius = abs(lightParam.z);
					float dis = length(lightPos - posStep);
					attenuation = (clamp((lightRadius * lightRadius) / (dis * dis) * thresAtRadius - thresAtRadius, 0.0f, 1.0f - thresAtRadius)) * lightColour.a;
					float sqrSinFragment = abs(1-cosFragment*cosFragment);
					float sqrSinHalfAngle = abs(1-cosHalfAngle*cosHalfAngle);
					float scaleAttenu = (sqrSinHalfAngle-sqrSinFragment)/sqrSinHalfAngle;
					attenuation = attenuation *  scaleAttenu * scaleAttenu;
				}
				else{
					attenuation = 0.0f;
				}
				
				if(toCastShadow>0){				
					//Shadow
					shadowMatrix = shadowMatrices[i*6];
					vec4 shadowProj = shadowMatrix*(vec4(posStep,1));
					vec3 shadowNDC = shadowProj.xyz / shadowProj.w;
					if(abs(shadowNDC.x)<1.0f&&abs(shadowNDC.y)<1.0f&&abs(shadowNDC.z)<1.0f){
						vec3 biasCoord = shadowNDC*0.5f + 0.5f;
						float shadowZ = texture(shadowTexs[i], biasCoord.xy).x;
						if(shadowZ<biasCoord.z){ //Inside Shadow
							shadow = 0.0f;
						}
					}
				}
			}
			
			//Calc color
			//clrLight+= (1.0f-dot(toEye, toLight))/2.0f * attenuation * strength * shadow * lightColour.rgb;
			clrLight+= stepLen * phaseHG(toLight, toEye) * attenuation * strength * shadow * lightColour.rgb;

			//Step
			posStep+=dirStep*stepLen;
			//stepLen = stepLen*expScale;
		}

		clr += clrLight*transmittance;
	}
	
	fragColor = vec4(clr,1);
}
