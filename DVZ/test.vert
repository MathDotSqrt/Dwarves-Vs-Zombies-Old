#version 330 core

in vec3 vert_pos;
in vec3 vert_col;


out vec3 frag_color;

void main(){
	frag_color = vert_col;
	gl_Position = vec4(vert_pos, 1);
}