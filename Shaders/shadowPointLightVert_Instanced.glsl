#version 330 core
layout(std140) uniform InstanceBlock{
	mat4 modelMatricesInstance[1024];
} instanceBlk;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

out vec3 worldPos;

void main(void){
	mat4 modelMatrix = instanceBlk.modelMatricesInstance[gl_InstanceID];
	vec4 wPos = modelMatrix*vec4(position,1.0);
	worldPos = wPos.xyz;
	gl_Position = (projMatrix*viewMatrix*modelMatrix)*vec4(position,1.0);
}