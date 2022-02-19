#version 330 core
uniform sampler2D screenTex;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;
void main(void){
	fragColour = texture(screenTex, IN.texCoord);
}