#version 330 core
#include "preamble.glsl"

layout(location = POSITION_ATTRIB_LOCATION) in vec4 vert_pos;

uniform mat4 MVP;

void main(){
	gl_Position = MVP * vec4(vert_pos.xyz, 1);
}