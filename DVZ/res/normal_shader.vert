#version 330 core

#include "preamble.glsl"

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

