#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

out vec3 worldPos;

void main(void){
	vec4 wPos = modelMatrix*vec4(position,1.0);
	worldPos = wPos.xyz;
	gl_Position = (projMatrix*viewMatrix*modelMatrix)*vec4(position,1.0);
}