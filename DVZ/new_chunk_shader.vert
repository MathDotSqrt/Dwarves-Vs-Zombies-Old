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

//uniform mat4 M;
uniform vec3 chunk_pos;
uniform mat4 VP;
uniform float time;

const float SPAWN_TIME = .5;
const float DEPTH = .1;

void main(){
	//function y = x - SPA
	//float y = min(DEPTH * (time * time - SPAWN_TIME), 0);
	float x_d = min(time - SPAWN_TIME, 0);
	float y = min(-100*(x_d*x_d), 0); //XD

	mat4 M = mat4(1);
	M[3].xyz = chunk_pos.xyz;// + vec3(-0.5, 0, -1);
	M[3].y += y;
	vec4 world_pos = M * vec4(vert_pos, 1);
	
	frag_pos = world_pos.xyz;
	frag_normal = vert_norm;
	frag_color = toLinear(vert_col);
	frag_uv = vert_uv;

	gl_Position = VP * world_pos;
}