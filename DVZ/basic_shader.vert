#version 330 core

#ifndef PREAMBLE_GLSL
#define PREAMBLE_GLSL

#define POSITION_ATTRIB_LOCATION 0
#define TEXCOORD_ATTRIB_LOCATION 1
#define NORMAL_ATTRIB_LOCATION 2
#define COLOR_ATTRIB_LOCATION 3
#endif



layout(location = POSITION_ATTRIB_LOCATION)
	in vec3 vert_pos;


out vec3 frag_color;

uniform mat4 model;
uniform vec3 color = vec3(1, 0, 1);


void main(){

	frag_color = color;
	gl_Position = model * vec4(vert_pos, 1);
}
