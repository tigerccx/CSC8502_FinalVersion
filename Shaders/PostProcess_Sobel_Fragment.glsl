#version 330 core

uniform sampler2D screenTex;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColor;

//Col first order
const float filterHorz[9] = 
	float[](-1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0);
const float filterVert[9] =
	float[](-1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0);

vec3 applyFilter(vec2 deltaHorz, vec2 deltaVert);

void main() {
	fragColor = vec4(applyFilter(dFdx(IN.texCoord), dFdy(IN.texCoord)),1);
}

vec3 applyFilter(vec2 deltaHorz, vec2 deltaVert) {
	vec3 rsltHorz = vec3(0, 0, 0);
	vec3 rsltVert = vec3(0, 0, 0);
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			vec2 offset = deltaHorz * (x - 1) + deltaVert * (y - 1);// vec2(, );
			vec3 tmp = texture2D(screenTex, IN.texCoord.xy + offset).xyz;
			rsltHorz += tmp * filterHorz[y * 3 + x];
			rsltVert += tmp * filterVert[y * 3 + x];
		}
	}
	return rsltHorz + rsltVert;
}