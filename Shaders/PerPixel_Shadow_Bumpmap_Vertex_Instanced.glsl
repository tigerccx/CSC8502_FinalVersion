#version 330 core
layout(std140) uniform InstanceBlock{
	mat4 modelMatricesInstance[1024];
} instanceBlk;
layout(std140) uniform CameraBlock{
	uniform mat4 viewMatrix;
	uniform mat4 projMatrix;
} cameraBlk;
uniform int bumpTex_use;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	//vec4 shadowProj;
} OUT;

void main() {
	OUT.colour = colour;
	OUT.texCoord = texCoord;
	
	mat4 modelMatrix = instanceBlk.modelMatricesInstance[gl_InstanceID];
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	OUT.normal = normalize(normalMatrix * normalize(normal));
	if(bumpTex_use>0){
		OUT.tangent = normalize(normalMatrix * normalize(tangent.xyz));
		OUT.binormal = cross(OUT.normal, OUT.tangent) * tangent.w;
	}
	
	vec4 worldPos = modelMatrix * vec4(position, 1);

	OUT.worldPos = worldPos.xyz;

	gl_Position = (cameraBlk.projMatrix * cameraBlk.viewMatrix) * worldPos;
	
	//vec3 viewDir = normalize(lightPos-worldPos.xyz);
	//vec4 pushVal = vec4(OUT.normal, 0) * dot(viewDir, OUT.normal); //offset
	//OUT.shadowProj = shadowMatrix*(worldPos+pushVal);
}