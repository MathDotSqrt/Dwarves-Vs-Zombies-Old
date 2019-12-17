#version 330 core

in vec2 frag_uv;

uniform sampler2D fbo;
uniform float time;
out vec4 final_color;

void main(){
	float t_step = 10 * time;
	//vec4 color = texture(fbo, frag_uv + 0.005*vec2( sin(t_step+20*frag_uv.x),cos(t_step+20*frag_uv.y)) ); 
	vec4 color = texture(fbo, frag_uv); 
	final_color = color; 
}