#version 330 core

uniform sampler2D screenTex;

uniform int stages;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColor;


void main() {
	fragColor = texture2D(screenTex, IN.texCoord.xy);
	vec3 fragColorRGB = fragColor.rgb;
	fragColorRGB = floor(fragColorRGB*(stages+1));
	fragColorRGB = fragColorRGB/(stages+1);
	fragColor = vec4(fragColorRGB, fragColor.a);
}