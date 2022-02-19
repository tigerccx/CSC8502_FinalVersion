#version 330 core
uniform mat4 modelMatrix;
layout(std140) uniform CameraBlock{
	uniform mat4 viewMatrix;
	uniform mat4 projMatrix;
} cameraBlk;

uniform mat4 viewMatrixMirrored;

in vec3 position;
in vec2 texCoord;

out Vertex{
	noperspective vec2 texCoord;
} OUT;

void main(void){
	mat4 mvp = cameraBlk.projMatrix*cameraBlk.viewMatrix*modelMatrix;
	gl_Position = mvp*vec4(position,1.0);
	mat4 mvpMirror = cameraBlk.projMatrix*viewMatrixMirrored*modelMatrix;
	vec4 screenPosMirror = mvpMirror*vec4(position,1.0);
	OUT.texCoord = ((screenPosMirror.xy/screenPosMirror.w)*0.5+0.5);
	//OUT.texCoord = texCoord;
	//OUT.texCoord.y = -OUT.texCoord.y+1;
}