#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 texMatrix = mat4(1.0);
uniform float texScale = 1;

in vec3 position;
in vec2 texCoord;

out Vertex{
	vec2 texCoord;
	vec4 colour;
} OUT;

void main(void){
	mat4 mvp = projMatrix*viewMatrix*modelMatrix;
	gl_Position = mvp*vec4(position,1.0);
	OUT.texCoord = (texMatrix*vec4(texCoord * texScale, 0.0, 1.0)).xy;
	OUT.colour = vec4(1,1,1,1);
}