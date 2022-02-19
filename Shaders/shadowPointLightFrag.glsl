#version 330 core

uniform vec3 lightPos;
uniform float lightRad;

in vec3 worldPos;

out vec4 fragColour;

void main(void){
	float dis = distance(lightPos, worldPos);
//	if(gl_Color.x<dis)
//		discard;
	fragColour = vec4(1);
	gl_FragDepth = dis/lightRad;
}