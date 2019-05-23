#version 330 core
in vec3 frag_color;

uniform vec3 u_col = vec3(1, 1, 1);


out vec4 final_color;

void main(){
	final_color = vec4(frag_color * u_col, 1);
}