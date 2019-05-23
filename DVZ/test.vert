#version 330 core

in vec3 vert_pos;
in vec3 vert_col;
in vec2 vert_tex_coord;

out vec3 frag_color;
out vec2 frag_tex_coord;

void main(){
	frag_color = vert_col;
	frag_tex_coord = vert_tex_coord;
	gl_Position = vec4(vert_pos, 1);
}