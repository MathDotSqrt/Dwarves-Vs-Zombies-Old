#version 330 core
#include "math.glsl"
#include "preamble.glsl"

layout(location = POSITION_ATTRIB_LOCATION) in vec4 vert_pos_light;
layout(location = NORMAL_ATTRIB_LOCATION) 	in vec3 vert_norm;
layout(location = BLOCK_TEXCOORD_ATTRIB_LOCATION) 	in vec4 vert_uv;

out vec3 frag_pos;
flat out int frag_light; 
out vec3 frag_normal;
out vec4 frag_uv;
out vec4 frag_pos_sun_space; 

//uniform mat4 M;
uniform float time;
uniform vec3 chunk_pos;
uniform mat4 VP;
uniform mat4 sun_VP;

const float SPAWN_TIME = .5;
const float DEPTH = .1;

void main(){
	/*CHUNK POS LIGHT DECODE*/
	vec3 vert_pos = vert_pos_light.xyz;
	/*CHUNK POS LIGHT DECODE*/

	/*CHUNK LOAD ANIMATION*/
	float x_d = min(time - SPAWN_TIME, 0);
	float y = min(-100*(x_d*x_d), 0); //XD
	/*CHUNK LOAD ANIMATION*/

	/*CHUNK TRANSFORMATION*/
	mat4 M = mat4(1);
	M[3].xyz = chunk_pos.xyz;// + vec3(-0.5, 0, -1);
	M[3].y += y;
	vec4 world_pos = M * vec4(vert_pos, 1);

	gl_Position = VP * world_pos;
	/*CHUNK TRANSFORMATION*/

	/*SUN SPACE MATRIX*/
	frag_pos_sun_space = sun_VP * world_pos;
	/*SUN SPACE MATRIX*/

	/*FRAG PASS*/
	frag_pos = world_pos.xyz;
	frag_light = int(vert_pos_light.w);
	frag_normal = vert_norm;
	frag_uv = vert_uv;
	/*FRAG PASS*/

}