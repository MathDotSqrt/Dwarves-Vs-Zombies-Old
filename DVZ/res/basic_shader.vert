#version 330 core

#include "preamble.glsl"

layout(location = POSITION_ATTRIB_LOCATION)
	in vec3 vert_pos;


out vec3 frag_color;

uniform mat4 MVP;
uniform vec3 color = vec3(1, 1, 1);


void main(){

	frag_color = color;
	gl_Position = MVP * vec4(vert_pos, 1);
}
