#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 modelMatrix;

uniform sampler2D offsetTex;
uniform int offsetTex_use = 0;
uniform sampler2D offsetTex1;
uniform int offsetTex1_use = 0;

uniform float texScale = 1;
uniform mat4 texMatrix = mat4(1.0);
uniform float texScale1 = 1;
uniform mat4 texMatrix1 = mat4(1.0);

//uniform float texScale2 = 1;
//uniform mat4 texMatrix2 = mat4(1.0);

uniform float offsetScale0 = 10;
uniform float offsetScale1 = 5;

uniform int bumpTex_use;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec4 tangent;
	vec3 worldPos;
} IN[];

out Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

void main()
{
    for(int i = 0; i < gl_in.length(); ++i) {   //We can iterate over the input array like so
		float offset0 = 0;
		float offset1 = 0;
		if(offsetTex_use>0){
			vec2 texCoord0 = (texMatrix*vec4(IN[i].texCoord*texScale,0,1)).xy;
			offset0 = texture(offsetTex, texCoord0).x;
			
		}
		if(offsetTex1_use>0){
			//vec2 texCoord1 = (texMatrix1*vec4(IN[i].texCoord*texScale1,0,1)).xy;
			vec2 texCoord1 = (texMatrix1*vec4(IN[i].texCoord*texScale1,0,1)).xy;
			offset1 = texture(offsetTex1, texCoord1).x;
		}
		//TODO: Need to recalculate normal tangent and ...
		mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
		
		OUT.normal = normalize(normalMatrix * normalize(IN[i].normal));
		if(bumpTex_use>0){
			OUT.tangent = normalize(normalMatrix * normalize(IN[i].tangent.xyz));
			OUT.binormal = cross(OUT.normal, OUT.tangent) * IN[i].tangent.w;
		}
		
		vec4 worldPos = modelMatrix * vec4(IN[i].worldPos, 1);

		OUT.worldPos = worldPos.xyz;
		
		OUT.colour = IN[i].colour;
		OUT.texCoord = IN[i].texCoord;
		
		gl_Position  = gl_in[i].gl_Position+vec4(0,offset0*offsetScale0+offset1*offsetScale1,0,0);
		EmitVertex(); //We've made a vertex, copy it out
	}
	EndPrimitive();
}
