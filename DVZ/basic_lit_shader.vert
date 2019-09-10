#version 330 core

#include "preamble.glsl"

layout(location = POSITION_ATTRIB_LOCATION) in vec3 vert_pos;
layout(location = NORMAL_ATTRIB_LOCATION) 	in vec3 vert_norm;

out vec3 frag_pos;
out vec3 frag_normal;

uniform mat4 M;
uniform mat4 VP;
uniform mat3 inverseTransposeMatrix;


void main(){
	vec4 world_pos = M * vec4(vert_pos, 1);
	
	frag_pos = world_pos.xyz;
	frag_normal = inverseTransposeMatrix * vert_norm;

	gl_Position = VP * world_pos;
}