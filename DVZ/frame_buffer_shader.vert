#version 330 
#include "preamble.glsl"

layout(location = POSITION_ATTRIB_LOCATION) in vec2 vert_pos;

out vec2 frag_uv;

void main(){
	frag_uv = vert_pos * .5 + .5;
	gl_Position = vec4(vert_pos, 0, 1);
}