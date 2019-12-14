#version 330 core
#include "math.glsl"

#define SPRITE_COL 2
#define MAX_LIGHTS 3

in vec3 frag_pos;
flat in int frag_light;
in vec3 frag_normal;
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

uniform vec3 camera_pos;
uniform sampler2DArray tex;
uniform sampler2DArray debug;

//uniform PointLight lights[3];
uniform DirLight dirLight;
uniform Ambient ambient;
uniform Fog fog;

const float fog_A = 4;
const float fog_B = 20;

const float fog_min = .005;
const float fog_max = 1550;

const float height_fog_start = 400;
const float min_step = 500;
const float max_step = 900;

float calc_dir_light(DirLight dlight){
	vec3 normalized_dir = normalize(dlight.dir);
	vec3 normalized_norm = normalize(frag_normal);
	return max(dot(-normalized_dir, normalized_norm), 0);
}

float calc_fog_factor(float start, float a, float dist){
	//float dist = gl_FragCoord.z / gl_FragCoord.w;

	float d = max(dist - start, 0);

	float factor = 1 / exp(d * d * a * a);
	return factor;
}

float calc_height_fog(float dist){
	//float dist = gl_FragCoord.z / gl_FragCoord.w;
	dist = max(dist - height_fog_start, 0);

	//float length = length(frag_pos-camera_pos);
	vec3 ray = frag_pos - camera_pos + vec3(.001, .001, .001);
	vec3 point = camera_pos;

	float numerator = fog_A*fog_B*(exp(ray.y/fog_B) - 1) * exp(-(point.y + ray.y)/fog_B);
	float denominator = ray.y;

	float fog_ammount = max(dist * numerator / denominator, 0);

	float fog_mix = ((fog_max - fog_ammount) / (fog_max - fog_min));
	return min(max(fog_mix, 0), 1); 
}

vec2 world_to_texcoord_space(){
	bool X = float_compare(abs(frag_normal.x), 1);
	bool Y = float_compare(abs(frag_normal.y), 1);
	vec2 texcoord =  X ? frag_pos.zy : frag_pos.xy; 
	texcoord = Y ? frag_pos.zx : texcoord; 
	texcoord.y = 1 - texcoord.y;
	return texcoord / 3;
}

void main(){
	/*LIGHT*/
	vec3 light_color = vec3(0);
	light_color += ambient.color * ambient.intensity * float(frag_light+1);
	//light_color += (float(frag_light * frag_light) + .1) / 66;

	float light_strength = calc_dir_light(dirLight);
	light_color += dirLight.color * light_strength;
	/*LIGHT*/

	/*TEXTURE*/
	vec2 texcoord = frag_uv.xy;
	float texindex = frag_uv.z + frag_uv.w * SPRITE_COL;
	vec3 texcoord3D = vec3(texcoord, texindex);
	//vec4 tex_color = toLinear(texture(tex, texcoord3D));

	vec2 world_texcoord = world_to_texcoord_space();
	vec3 world_texcoord3D = vec3(world_texcoord, texindex);
	vec4 tex_color = toLinear(texture(debug, world_texcoord3D)); 
	/*TEXTURE*/ 


	vec3 final_light_color = tex_color.rgb * light_color;

	/*FOG*/
	float dist = length(frag_pos-camera_pos);//gl_FragCoord.z / gl_FragCoord.w;
	float f_d = calc_fog_factor(fog.start_dist, fog.attenuation, dist);
	float f_h = calc_height_fog(dist);

	float fog_mix = smoothstep(min_step, max_step, dist);
	float final_mix = mix(f_h, 0, fog_mix);
	

	//float final_mix = (f_d * alpha+f_h*beta) / (alpha+beta);
	vec3 frag_color = mix(fog.color, final_light_color, final_mix);
	/*FOG*/

	final_color = vec4(frag_color, 1);
	final_color = toGamma(final_color);

	if(tex_color.a < .5){
		discard;
	}
}