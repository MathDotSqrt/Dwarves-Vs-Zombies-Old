#version 330 core
#include "math.glsl"
#include "preamble.glsl"

layout(location = POSITION_ATTRIB_LOCATION) in vec3 vert_pos;
layout(location = NORMAL_ATTRIB_LOCATION) 	in vec3 vert_norm;
layout(location = COLOR_ATTRIB_LOCATION) 	in vec3 vert_col;
layout(location = BLOCK_TEXCOORD_ATTRIB_LOCATION) 	in vec4 vert_uv;

out vec3 frag_pos;
out vec3 frag_normal;
out vec3 frag_color;
out vec4 frag_uv;
out vec4 frag_view_space;

//uniform mat4 M;
uniform vec3 chunk_pos;
uniform mat4 V;
uniform mat4 P;

void main(){
	mat4 M = mat4(1);
	M[3].xyz = chunk_pos.xyz;
	

	vec4 world_pos = M * vec4(vert_pos, 1);
	vec4 MV = V * world_pos;
	
	frag_pos = world_pos.xyz;
	frag_normal = vert_norm;
	frag_color = toLinear(vert_col);
	frag_uv = vert_uv;

	frag_view_space = MV;


	gl_Position = P * MV;
}