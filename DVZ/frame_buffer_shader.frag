#version 330 

in vec2 frag_uv;

uniform sampler2D fbo;

out vec4 final_color;

void main(){
	//vec4 color = texture(fbo, 0, 1) + 1;
	final_color = vec4(1, 0, 0, 0); 
}