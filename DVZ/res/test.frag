#version 330 core
in vec3 frag_color;
in vec2 frag_tex_coord;

uniform vec3 u_col = vec3(1, 1, 1);
uniform sampler2D tex1;
uniform sampler2D tex2;

out vec4 final_color;

void main(){
	vec4 tex_color = mix(texture(tex1, frag_tex_coord), texture(tex2, frag_tex_coord), 0.25);
	final_color = tex_color * vec4(frag_color * u_col, 1);
}