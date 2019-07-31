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
layout(location = NORMAL_ATTRIB_LOCATION)
	in vec3 vert_norm;

out vec3 frag_color;

uniform mat4 MVP;
uniform mat3 inverseTransposeMatrix;

void main(){
	frag_color = inverseTransposeMatrix * normalize(vert_norm);
	gl_Position = MVP * vec4(vert_pos, 1);
}

