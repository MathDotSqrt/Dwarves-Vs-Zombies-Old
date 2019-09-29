#version 330 core

in vec3 frag_pos;
in vec3 frag_normal;
in vec3 frag_color;

out vec4 final_color;

uniform vec3 camera_pos;

uniform vec3 point_light_position;
uniform vec3 point_light_color = vec3(1, 1, 1);
uniform float point_light_intensity = 0;

//uniform vec3 diffuse_color = vec3(1, 1, 1);
uniform vec3 specular_color = vec3(1, 1, 1);
uniform float shinyness = 1;

void main(){
	vec3 frag_to_point = point_light_position - frag_pos;
	vec3 unitPointDir = normalize(frag_to_point);
	vec3 unitCameraDir = normalize(camera_pos - frag_pos);
	vec3 unitNormal = normalize(frag_normal);

	float lightStrength = max(dot(unitNormal, unitPointDir), 0);

	vec3 lightRay = reflect(-unitPointDir, unitNormal);
	float specularStrength = pow(max(dot(unitCameraDir, lightRay), 0), shinyness);

	float light_attenuation = min(point_light_intensity / dot(frag_to_point, frag_to_point), 1);

	vec3 lightColor = vec3(0, 0, 0);
	lightColor += point_light_color * lightStrength * light_attenuation * .7;
	lightColor += point_light_color * specular_color * specularStrength * light_attenuation;
	//lightColor += vec3(.7, .7, .6) *  .9;

	vec3 ambientColor = vec3(1, 1, 1) * .2;
	lightColor += ambientColor;


	final_color = vec4(frag_color * lightColor, 1);
}