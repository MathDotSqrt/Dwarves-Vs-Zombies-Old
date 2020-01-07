#version 330 core

#include "preamble.glsl"

layout(location = POSITION_ATTRIB_LOCATION) in vec3 vert_pos;

out vec3 frag_color;


void main(){
	frag_color = vert_col;
	frag_tex_coord = vert_tex_coord;
	gl_Position = vec4(vert_pos, 1);
}