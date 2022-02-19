#version 330 core

uniform sampler2D screenTex;
uniform vec2 offset;
uniform float blend;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main() {
	vec2 doffset = (dFdx(IN.texCoord)+dFdy(IN.texCoord)) * offset;
	vec4 clr0 = texture2D(screenTex, IN.texCoord.xy);
	vec4 clr1 = texture2D(screenTex, IN.texCoord.xy + doffset);
	fragColor = clr0* blend + clr1* (1-blend);

}