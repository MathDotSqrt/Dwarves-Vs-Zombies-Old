#version 330 core
#include "math.glsl"
#include "preamble.glsl"


layout(location = POSITION_ATTRIB_LOCATION) in vec3 vert_pos;
layout(location = NORMAL_ATTRIB_LOCATION) 	in vec3 vert_norm;
layout(location = COLOR_ATTRIB_LOCATION) 	in vec3 vert_col;

out vec3 frag_pos;
out vec3 frag_normal;
out vec3 frag_color;

//uniform mat4 M;
uniform vec3 pos;
uniform mat4 VP;
//uniform mat3 inverseTransposeMatrix;


void main(){
	mat4 M = mat4(1);
	M[3][0] = pos.x;
	M[3][1] = pos.y;
	M[3][2] = pos.z;

	vec4 world_pos = M * vec4(vert_pos, 1);
	
	frag_pos = world_pos.xyz;
	frag_normal = vert_norm;
	frag_color = toLinear(vert_col);

	gl_Position = VP * world_pos;
}