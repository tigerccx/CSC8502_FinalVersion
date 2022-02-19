#version 330 core
#include "Lighting/Lighting_PerPixel.glsl"

//Shadow
uniform int castShadow[NCLGL_LIGHT_MAX_PERPIXEL];
uniform sampler2D shadowTexs[NCLGL_LIGHT_MAX_PERPIXEL];
uniform samplerCube shadowCubeTexs[NCLGL_LIGHT_MAX_PERPIXEL];
//uniform samplerCube shadowCubeTexs;
uniform mat4 shadowMatrices[NCLGL_LIGHT_MAX_PERPIXEL*6];

//Material
uniform int diffuseTex_use;
uniform sampler2D diffuseTex;
uniform int bumpTex_use;
uniform sampler2D bumpTex;
uniform int metallicTex_use;
uniform sampler2D metallicTex;
uniform float metallic;
uniform int smoothnessTex_use;
uniform sampler2D smoothnessTex;
uniform int isRoughnessTex;
uniform float smoothness;
uniform int AOTex_use;
uniform sampler2D AOTex;
uniform float ambientComponent;

uniform int useReflection = 0;
uniform float reflectionCoeff = 0.5f;
uniform int cubeTex_use;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;


uniform float texScale = 1;
uniform mat4 texMatrix = mat4(1.0);
uniform float texScale1 = 1;
uniform mat4 texMatrix1 = mat4(1.0);


in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	//vec3 samplerDir;
	//vec4 shadowProj;
} IN;

out vec4 fragColour;

void main() {	
	vec3 toEye = normalize(cameraPos - IN.worldPos);
	vec2 texCoord = (texMatrix1*vec4(IN.texCoord*texScale1,0,1)).xy;
	
	vec4 diffuseClr = vec4(1,1,1,1);
	if(diffuseTex_use>0){
		diffuseClr = texture(diffuseTex, texCoord);
	}
	
	vec3 bumpNormal = vec3(0,0,0);
	if(bumpTex_use>0){
		mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
		bumpNormal = texture(bumpTex, texCoord).rgb;
		bumpNormal = normalize(TBN*normalize(bumpNormal*2.0f-1.0f));
	}
	else{
		bumpNormal = IN.normal;
	}
	
	float valMetallic = 0;
	if(metallicTex_use>0){
		valMetallic = texture(metallicTex, IN.texCoord).a;
	}
	else{
		valMetallic = metallic;
	}
	
	float roughness = 0;
	if(smoothnessTex_use>0){
		roughness = texture(smoothnessTex, IN.texCoord).a;
		if(isRoughnessTex==0)
			roughness = 1.0f-roughness;
	}
	else{
		roughness = 1.0f-smoothness;
	}
	
	float ao = 1.0f;
	if(AOTex_use>0){
		ao = texture(AOTex, IN.texCoord).a;
	}
	
	fragColour = vec4(0, 0, 0, 0);

	for (int i = 0; i < lightCount; ++i) {
		int lightType = lights[i];
		
		vec3 clr = vec3(0,0,0);
		
		//Shadow
		float shadow = 1.0f;
		vec3 world2Light = lightPositions[i]-IN.worldPos.xyz;
		vec3 viewDir = normalize(world2Light);
		vec4 pushVal = vec4(IN.normal, 0) * dot(viewDir, IN.normal); //vec4(bumpNormal, 0) * dot(viewDir, bumpNormal); //offset
		mat4 shadowMatrix;
		int toCastShadow = castShadow[i];
		
		if(lightType == 0){ //Directional Light
			CalculateDirectionalLightPBR(toEye, IN.worldPos, bumpNormal,
								diffuseClr.rgb, 
								valMetallic, roughness,
								lightColours[i], lightPositions[i], lightParams[i], 
								clr);
			if(toCastShadow>0){
				//Shadow
				shadowMatrix = shadowMatrices[i*6];
				vec4 shadowProj = shadowMatrix*(vec4(IN.worldPos,1));
				vec3 shadowNDC = shadowProj.xyz;
				if(abs(shadowNDC.x)<1.0f&&abs(shadowNDC.y)<1.0f&&abs(shadowNDC.z)<1.0f){
					vec3 biasCoord = shadowNDC*0.5f + 0.5f;
					float shadowZ = texture(shadowTexs[i], biasCoord.xy).x;
					if(shadowZ<(biasCoord.z-0.0002)){ //Inside Shadow  //TODO: Find out how to push in a better way
					//if(shadowZ<biasCoord.z){ //Inside Shadow
						shadow = 0.0f;
						//fragColour = vec4(1,1,1,1);
					}
					//fragColour = vec4(biasCoord.xy,biasCoord.z,1);
					//fragColour = vec4(shadowProj.w,shadowProj.w,shadowProj.w,1);
					//return;
				}
			}
		}
		else if (lightType == 1) { //Point Light
			CalculatePointLightPBR(toEye, IN.worldPos, bumpNormal,
								diffuseClr.rgb, 
								valMetallic, roughness,
								lightColours[i], lightPositions[i], lightParams[i], 
								clr);
			if(toCastShadow>0){
				//Shadow
				float biasZ = distance(IN.worldPos+pushVal.xyz, lightPositions[i])/lightParams[i].x;
				//vec3 light2World = viewDir;
				vec3 light2World = world2Light;
				light2World.z = -light2World.z;
				float shadowZ = texture(shadowCubeTexs[i], light2World).x;
				//float shadowZ = texture(shadowCubeTexs, light2World).r;
				if(shadowZ<biasZ){ //Inside Shadow
					shadow = 0.0f;
				}
			}
		}
		else if (lightType == 2) { //Spot Light
			CalculateSpotLightPBR(toEye, IN.worldPos, bumpNormal,
								diffuseClr.rgb, 
								valMetallic, roughness,
								lightColours[i], lightPositions[i], lightParams[i], 
								clr);
			if(toCastShadow>0){				
				//Shadow
				shadowMatrix = shadowMatrices[i*6];
				vec4 shadowProj = shadowMatrix*(vec4(IN.worldPos,1)+pushVal);
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
		
		fragColour += vec4(clr,0)*shadow;
	}
	
	fragColour += vec4(ambientColour.xyz*diffuseClr.rgb*ambientComponent*ao, 0);
	
	vec4 reflectTex = vec4(0,0,0,0);
	float alpha = diffuseClr.a;
	if(useReflection>0 && cubeTex_use>0){
		vec3 reflectDir = reflect(-toEye, bumpNormal);
		reflectTex = texture(cubeTex, reflectDir);
		float EdotN = dot(toEye, bumpNormal);
		float fresnelCoeff = ((1-EdotN)*0.9+0.1);
		alpha = fresnelCoeff * alpha;
		fragColour += reflectTex*reflectionCoeff;
	}
	
	fragColour.a = alpha;
}