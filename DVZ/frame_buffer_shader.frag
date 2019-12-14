#version 330 core

in vec2 frag_uv;

uniform sampler2D fbo;

out vec4 final_color;

void main(){
	vec4 color = texture(fbo, frag_uv); 
	final_color = color; 
}