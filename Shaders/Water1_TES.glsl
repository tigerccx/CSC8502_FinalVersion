#version 400 core
layout(triangles, equal_spacing, ccw) in;
uniform mat4 modelMatrix;
layout(std140) uniform CameraBlock{
	uniform mat4 viewMatrix;
	uniform mat4 projMatrix;
} cameraBlk;

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
} OUT; //Array of data to send to tessellator

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}
vec4 interpolate4D(vec4 v0, vec4 v1, vec4 v2)
{
    return vec4(gl_TessCoord.x) * v0 + vec4(gl_TessCoord.y) * v1 + vec4(gl_TessCoord.z) * v2;
}

void main()
{
    vec3  p0   = gl_TessCoord.x * gl_in[0].gl_Position.xyz;
    vec3  p1   = gl_TessCoord.y * gl_in[1].gl_Position.xyz;
	vec3  p2   = gl_TessCoord.z * gl_in[2].gl_Position.xyz;
    
	OUT.colour = interpolate4D(IN[0].colour,IN[1].colour,IN[2].colour);
	OUT.texCoord = interpolate2D(IN[0].texCoord,IN[1].texCoord,IN[2].texCoord);
	OUT.normal = interpolate3D(IN[0].normal,IN[1].normal,IN[2].normal);
	OUT.tangent = interpolate4D(IN[0].tangent,IN[1].tangent,IN[2].tangent);//vec4(interpolate3D(IN[0].tangent.xyz,IN[1].tangent.xyz,IN[2].tangent.xyz), tangent.w);
	OUT.worldPos = interpolate3D(IN[0].worldPos,IN[1].worldPos,IN[2].worldPos);
	
	vec3 combinedPos = p0+p1+p2;
	
    gl_Position = cameraBlk.projMatrix * cameraBlk.viewMatrix * modelMatrix * vec4(combinedPos, 1);
}