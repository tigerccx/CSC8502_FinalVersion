#version 330 core
layout(std140) uniform InstanceBlock{
	mat4 modelMatricesInstance[1024];
} instanceBlk;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

void main(void){
	mat4 modelMatrix = instanceBlk.modelMatricesInstance[gl_InstanceID];
	gl_Position = (projMatrix*viewMatrix*modelMatrix)*vec4(position,1.0);
}