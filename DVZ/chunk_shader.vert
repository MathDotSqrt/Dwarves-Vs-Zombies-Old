#version 330 core

#include "preamble.glsl"

layout(location = POSITION_ATTRIB_LOCATION) in vec3 vert_pos;
layout(location = NORMAL_ATTRIB_LOCATION) 	in vec3 vert_norm;
layout(location = COLOR_ATTRIB_LOCATION) 	in vec3 vert_col;

out vec3 frag_pos;
out vec3 frag_normal;
out vec3 frag_color;

uniform mat4 M;
uniform mat4 VP;
uniform mat3 inverseTransposeMatrix;


void main(){
	vec4 world_pos = M * vec4(vert_pos, 1);
	
	frag_pos = world_pos.xyz;
	frag_normal = inverseTransposeMatrix * vert_norm;
	frag_color = vert_col;

	gl_Position = VP * world_pos;
}