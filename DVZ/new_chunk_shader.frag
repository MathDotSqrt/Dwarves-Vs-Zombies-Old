#version 330 core
#include "math.glsl"

#define SPRITE_COL 16
#define MAX_LIGHTS 3

in vec3 frag_pos;
in vec3 frag_normal;
in vec3 frag_color;
in vec4 frag_uv;

out vec4 final_color;

struct PointLight {
	vec3 pos;
	vec3 color;
	float intensity;	
};

struct DirLight {
	vec3 dir;
	vec3 color;
	float intensity;
};

struct Ambient {
	vec3 color;
	float intensity;
};

struct Fog {
	vec3 color;
	float start_dist;
	float attenuation;
};

//uniform PointLight lights[3];
uniform DirLight dirLight;
uniform Ambient ambient;
uniform Fog fog;

uniform vec3 camera_pos;

uniform sampler2DArray tex;

float calc_dir_light(DirLight dlight){
	vec3 normalized_dir = normalize(dlight.dir);
	vec3 normalized_norm = normalize(frag_normal);
	return max(dot(-normalized_dir, normalized_norm), 0);
}

float calc_fog_factor(float start, float a){
	float dist = gl_FragCoord.z / gl_FragCoord.w;

	float d = max(dist - start, 0);

	float factor = 1 / exp(d * d * a * a);
	return factor;
}

void main(){
	/*LIGHT*/
	vec3 light_color = vec3(0);
	light_color += ambient.color * ambient.intensity;

	float light_strength = calc_dir_light(dirLight);
	light_color += dirLight.color * light_strength;
	/*LIGHT*/

	/*TEXTURE*/
	vec2 texcoord = frag_uv.xy;
	float texindex = frag_uv.z + frag_uv.w * SPRITE_COL;
	vec3 texcoord3D = vec3(texcoord, texindex);
	vec4 tex_color = toLinear(texture(tex, texcoord3D));
	/*TEXTURE*/

	vec3 final_light_color = tex_color.rgb * light_color;

	/*FOG*/
	float f = calc_fog_factor(fog.start_dist, fog.attenuation);
	vec3 frag_color = mix(fog.color, final_light_color, f);
	/*FOG*/

	final_color = vec4(frag_color, 1);
	final_color = toGamma(final_color);

	if(tex_color.a < .5){
		discard;
	}
}