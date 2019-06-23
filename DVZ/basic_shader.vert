#version 330 core

#ifndef PREAMBLE_GLSL
#define PREAMBLE_GLSL

#define POSITION_ATTRIB_LOCATION 0
#define TEXCOORD_ATTRIB_LOCATION 1
#define NORMAL_ATTRIB_LOCATION 2
#define COLOR_ATTRIB_LOCATION 3
#endif



layout(location = POSITION_ATTRIB_LOCATION)
	in vec2 vert_pos;


out vec3 frag_color;

uniform vec3 position = vec3(0, 0, 0);
uniform vec3 scale = vec3(.1, .1, .1);
uniform vec3 color = vec3(1, 0, 1);


void main(){

	frag_color = color;
	gl_Position = vec4(vert_pos * scale.xy + position.xy, 0, 1);
}
