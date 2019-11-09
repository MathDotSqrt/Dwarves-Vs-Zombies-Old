#version 330 core

#include "math.glsl"

in vec3 frag_pos;
in vec3 frag_normal;
in vec3 frag_color;
in vec2 frag_uv;

out vec4 final_color;

uniform vec3 camera_pos;

uniform vec3 point_light_position;
uniform vec3 point_light_color = vec3(1, 1, 1);
uniform float point_light_intensity = 0;

//uniform vec3 diffuse_color = vec3(1, 1, 1);
uniform vec3 specular_color = vec3(1, 1, 1);
uniform float shinyness = 1;

uniform sampler2DArray texAtlas;

vec4 fog_color = vec4(.15, .15, .15, 1);

float fog_A = .3;
float fog_B = 8;

float fog_min = .001;
float fog_max = 15;

float fog_mix(vec3 camera_pos, vec3 frag_pos){
	float length = length(frag_pos-camera_pos);
	vec3 ray = frag_pos - camera_pos + vec3(.001, .001, .001);
	vec3 point = camera_pos;

	float numerator = fog_A*fog_B*(exp(ray.y/fog_B) - 1) * exp(-(point.y + ray.y)/fog_B);
	float denominator = ray.y;

	return max(length * numerator / denominator, 0);
}

void main(){
	vec3 frag_to_point = point_light_position - frag_pos;
	vec3 unitPointDir = normalize(frag_to_point);
	vec3 unitCameraDir = normalize(camera_pos - frag_pos);
	vec3 unitNormal = normalize(frag_normal);

	float lightStrength = max(dot(unitNormal, unitPointDir), 0);

	vec3 lightRay = reflect(-unitPointDir, unitNormal);
	float specularStrength = pow(max(dot(unitCameraDir, lightRay), 0), 1);

	float light_attenuation = min(point_light_intensity / dot(frag_to_point, frag_to_point), shinyness);

	vec3 lightColor = vec3(0, 0, 0);
	lightColor += point_light_color * lightStrength * light_attenuation * .7;
	lightColor += point_light_color * specular_color * specularStrength * light_attenuation;
	//lightColor += vec3(.7, .7, .6) *  .9;

	vec3 ambientColor = vec3(1, 1, 1) * .5;
	lightColor += ambientColor;


	final_color = vec4(frag_color * lightColor, 1);
	//final_color = toGamma(final_color) + (fog_color*fog_density(0));

	float fog = fog_mix(camera_pos, frag_pos);
	float mix_factor = 1 - (fog_max - fog) / (fog_max - fog_min);
	//final_color = toGamma(mix(final_color, fog_color, min(mix_factor, .7))); 
	//final_color = toGamma(fog_color * mix_factor);

	vec4 tex_color = texture(texAtlas, vec3(frag_uv.xy, 0));
	final_color = tex_color; 
}