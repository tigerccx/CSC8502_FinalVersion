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
uniform vec3 diffuseComponent;
uniform vec3 specularComponent;
uniform float ambientComponent;
uniform float specularGloss;
uniform int useReflection = 0;
uniform float reflectionCoeff = 0.5f;
uniform int cubeTex_use;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;

in Vertex{
	vec3 colour;
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
	
	vec4 diffuseClr = vec4(1,1,1,1);
	if(diffuseTex_use>0){
		diffuseClr = texture(diffuseTex, IN.texCoord);
	}
	
	vec3 bumpNormal = vec3(0,0,0);
	if(bumpTex_use>0){
		mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
		bumpNormal = texture(bumpTex, IN.texCoord).rgb;
		bumpNormal = normalize(TBN*normalize(bumpNormal*2.0f-1.0f));
	}
	else{
		bumpNormal = IN.normal;
	}
	
	fragColour = vec4(0, 0, 0, 0);

	for (int i = 0; i < lightCount; ++i) {
		int lightType = lights[i];
		
		vec3 diffuse;
		vec3 specular;
		
		//Shadow
		float shadow = 1.0f;
		vec3 world2Light = lightPositions[i]-IN.worldPos.xyz;
		vec3 viewDir = normalize(world2Light);
		vec4 pushVal = vec4(IN.normal, 0) * dot(viewDir, IN.normal); //vec4(bumpNormal, 0) * dot(viewDir, bumpNormal); //offset
		mat4 shadowMatrix;
		int toCastShadow = castShadow[i];
		
		if(lightType == 0){ //Directional Light
			CalculateDirectionalLight(toEye, bumpNormal,
										diffuseClr.rgb, specularGloss,
										lightColours[i], lightPositions[i], lightParams[i],
										diffuse, specular);
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
			CalculatePointLight(toEye, IN.worldPos, bumpNormal,
								diffuseClr.rgb, specularGloss,
								lightColours[i], lightPositions[i], lightParams[i], 
								diffuse, specular);
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
			CalculateSpotLight(toEye, IN.worldPos, bumpNormal,
								diffuseClr.rgb, specularGloss,
								lightColours[i], lightPositions[i], lightParams[i], 
								diffuse, specular);
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
		
		fragColour += vec4(diffuse*diffuseComponent + specular*specularComponent, 0)*shadow;
	}
	
	vec4 lightingClr = vec4(ambientColour.xyz*diffuseClr.rgb*ambientComponent, 0);
	
	vec4 reflectTex = vec4(0,0,0,0);
	if(useReflection>0 && cubeTex_use>0){
		vec3 reflectDir = reflect(-toEye, bumpNormal);
		reflectTex = texture(cubeTex, reflectDir);
		fragColour += lightingClr*(1-reflectionCoeff) + reflectTex*reflectionCoeff;
		//fragColour = vec4(1,1,1,1);
		//return;
	}

	fragColour += lightingClr;
	
	fragColour.a = diffuseClr.a;
}