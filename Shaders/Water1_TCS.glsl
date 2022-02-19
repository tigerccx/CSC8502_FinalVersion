#version 400
layout(vertices=4) out;

uniform float tessLevelInner = 400;
uniform float tessLevelOuter = 402;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec4 tangent;
	vec3 worldPos;
} IN[]; //An array of data from vertex shader
out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec4 tangent;
	vec3 worldPos;
} OUT[]; //Array of data to send to tessellator

void main()
{
    // Pass along the vertex position unmodified
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;    
	OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;    
	OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;    
	OUT[gl_InvocationID].tangent = IN[gl_InvocationID].tangent;    
	OUT[gl_InvocationID].worldPos = IN[gl_InvocationID].worldPos;    
	OUT[gl_InvocationID].colour = IN[gl_InvocationID].colour;    
	
	gl_TessLevelInner[0] = tessLevelInner;
	gl_TessLevelInner[1] = tessLevelInner;
	
    gl_TessLevelOuter[0] = tessLevelOuter;
    gl_TessLevelOuter[1] = tessLevelOuter;
	gl_TessLevelOuter[2] = tessLevelOuter;
	gl_TessLevelOuter[3] = tessLevelOuter;
}