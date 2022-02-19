#version 330 core
uniform sampler2D diffuseTex;

in Vertex{
	noperspective vec2 texCoord;
} IN;

out vec4 fragColour;
void main(void){
	fragColour = texture(diffuseTex, IN.texCoord);

	//fragColour = vec4(IN.texCoord,0,1); // IN.colour ;//
}