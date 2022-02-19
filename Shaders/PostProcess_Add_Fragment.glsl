#version 330 core

uniform sampler2D texA;
uniform sampler2D texB;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main() {
	fragColor = texture2D(texA, IN.texCoord.xy)+texture2D(texB, IN.texCoord.xy);
}