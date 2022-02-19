#version 330 core
uniform sampler2D diffuseTex;
//uniform float vtxBlendScale;

//uniform float minAlpha;

in Vertex{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;
void main(void){
	fragColour = texture(diffuseTex, IN.texCoord) *IN.colour; // IN.colour ;//
	
	if(fragColour.a<0.1f)
		discard;
		
	//fragColour = vec4(fragColour.r, fragColour.g, fragColour.b, 1);
	//fragColour = vec4(IN.texCoord,0,1);
}