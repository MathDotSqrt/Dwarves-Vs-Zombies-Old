#version 330 core
#include "preamble.glsl"

layout(location = POSITION_ATTRIB_LOCATION) in vec4 vert_pos;

uniform vec3 chunk_pos;
uniform mat4 VP;

void main(){

	mat4 M = mat4(1);
	M[3].xyz = chunk_pos.xyz;
	vec4 world_pos = M * vec4(vert_pos.xyz, 1);

	gl_Position = VP * world_pos;
}