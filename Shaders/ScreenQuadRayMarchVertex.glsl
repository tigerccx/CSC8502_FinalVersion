#version 330 core

uniform mat4 viewInverseMatrix;
uniform mat4 projInverseMatrix;
uniform vec3 cameraPos;
uniform float cameraNear;

in vec3 position;
in vec2 texCoord;

out Vertex{
	vec2 texCoord;
	vec3 dirRay;
} OUT;

void main(void){
	gl_Position = vec4(position,1.0);
	OUT.dirRay = (projInverseMatrix*vec4(position.xy, 0, 0)).xyz;
	OUT.dirRay.z = -cameraNear;
	OUT.dirRay = (viewInverseMatrix*vec4(OUT.dirRay, 0)).xyz;
	OUT.texCoord = texCoord;
}