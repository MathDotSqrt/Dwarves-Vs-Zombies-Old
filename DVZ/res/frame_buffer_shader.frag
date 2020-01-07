#version 330 core

in vec2 frag_uv;

uniform sampler2D fbo;
uniform float time;
out vec4 final_color;

void main(){
	float t_step = 10 * time;

	vec4 color = texture(fbo, frag_uv); 
	final_color = color;

	//final_color = vec4(vec3(texture(fbo, frag_uv).r), 1); 
}