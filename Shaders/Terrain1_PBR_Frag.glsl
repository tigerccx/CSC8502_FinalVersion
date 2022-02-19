#version 330 core
#include "Lighting/Lighting_PerPixel.glsl"

//Shadow
uniform int castShadow[NCLGL_LIGHT_MAX_PERPIXEL];
uniform sampler2D shadowTexs[NCLGL_LIGHT_MAX_PERPIXEL];
uniform samplerCube shadowCubeTexs[NCLGL_LIGHT_MAX_PERPIXEL];
//uniform samplerCube shadowCubeTexs;
uniform mat4 shadowMatrices[NCLGL_LIGHT_MAX_PERPIXEL*6];

//Material
uniform sampler2D diffuseTex0;
uniform sampler2D bumpTex0;
uniform float metallic0;
uniform float smoothness0;
uniform sampler2D diffuseTex1;
uniform sampler2D bumpTex1;
uniform float metallic1;
uniform float smoothness1;
uniform sampler2D diffuseTex2;
uniform sampler2D bumpTex2;
uniform float metallic2;
uniform float smoothness2;

uniform float thres00;
uniform float thres01;
uniform float thres10;
uniform float thres11;

uniform float ambientComponent;

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
} IN;

out vec4 fragColour;

void main() {
	vec3 toEye = normalize(cameraPos - IN.worldPos);
	
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
	
	vec4 diffuseClr = vec4(1,1,1,1);
	vec3 bumpNormal = vec3(0,0,0);
	float roughness = 0.0f;
	float valMetallic = 0.0f;
	
	float heightW = IN.worldPos.y;
	if(heightW<thres00){
		diffuseClr = texture(diffuseTex0, IN.texCoord);
		bumpNormal = texture(bumpTex0, IN.texCoord).rgb;
		roughness = 1.0f-smoothness0;
		valMetallic = metallic0;
	}
	else if(thres00<=heightW&&heightW<thres01){
		float coeff = (thres01-heightW)/(thres01-thres00);
		diffuseClr = coeff*texture(diffuseTex0, IN.texCoord) + (1-coeff)*texture(diffuseTex1, IN.texCoord);
		bumpNormal = coeff*texture(bumpTex0, IN.texCoord).rgb + (1-coeff)*texture(bumpTex1, IN.texCoord).rgb;
		roughness = 1.0f-(coeff*smoothness0+(1-coeff)*smoothness1);
		valMetallic = coeff*metallic0+(1-coeff)*metallic1;
	}
	else if(thres01<=heightW&&heightW<thres10){
		diffuseClr = texture(diffuseTex1, IN.texCoord);
		bumpNormal = texture(bumpTex1, IN.texCoord).rgb;
		roughness = 1.0f-smoothness1;
		valMetallic = metallic1;
	}
	else if(thres10<=heightW&&heightW<thres11){
		float coeff = (thres11-heightW)/(thres11-thres10);
		diffuseClr = coeff*texture(diffuseTex1, IN.texCoord) + (1-coeff)*texture(diffuseTex2, IN.texCoord);
		bumpNormal = coeff*texture(bumpTex1, IN.texCoord).rgb + (1-coeff)*texture(bumpTex2, IN.texCoord).rgb;
		roughness = 1.0f-(coeff*smoothness1+(1-coeff)*smoothness2);
		valMetallic = coeff*metallic1+(1-coeff)*metallic2;
	}
	else{
		diffuseClr = texture(diffuseTex2, IN.texCoord);
		bumpNormal = texture(bumpTex2, IN.texCoord).rgb;
		roughness = 1.0f-smoothness2;
		valMetallic = metallic2;
	}
	
	bumpNormal = normalize(TBN*normalize(bumpNormal*2.0f-1.0f));
	
	fragColour = vec4(0, 0, 0, 0);

	for (int i = 0; i < lightCount; ++i) {
		int lightType = lights[i];
		
		vec3 clr;
		
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
					if(shadowZ<(biasCoord.z-0.00002)){ 
						shadow = 0.0f;
					}
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
				vec3 light2World = world2Light;
				light2World.z = -light2World.z;
				float shadowZ = texture(shadowCubeTexs[i], light2World).x;
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
	
	fragColour += vec4(ambientColour.xyz*diffuseClr.rgb*ambientComponent, 0);
	
	vec4 reflectTex = vec4(0,0,0,0);
	if(useReflection>0 && cubeTex_use>0){
		vec3 reflectDir = reflect(-toEye, bumpNormal);
		reflectTex = texture(cubeTex, reflectDir);
		fragColour += reflectTex*reflectionCoeff;
	}
	
	fragColour.a = 1;
}