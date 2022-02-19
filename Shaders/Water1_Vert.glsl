#version 400
uniform mat4 modelMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec4 tangent;
	vec3 worldPos;
} OUT;


void main()
{
    OUT.colour = colour;
	OUT.texCoord = texCoord;
	OUT.normal = normal;
	OUT.tangent = tangent;
	OUT.worldPos = position.xyz;

	gl_Position = vec4(position, 1);
}
