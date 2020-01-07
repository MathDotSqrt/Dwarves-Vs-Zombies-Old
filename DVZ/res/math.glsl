
#define GAMMA 2.2

float toGamma(float x){
	return pow(x, 1/GAMMA);
}

vec3 toGamma(vec3 x){
	return pow(x, vec3(1/GAMMA));
}

vec4 toGamma(vec4 x){
	return pow(x, vec4(1/GAMMA));
}

float toLinear(float x){
	return pow(x, GAMMA);
}

vec3 toLinear(vec3 x){
	return pow(x, vec3(GAMMA));
}

vec4 toLinear(vec4 x){
	return pow(x, vec4(GAMMA));
}

bool float_compare(float f, float value, float epsilon=.01){
	return (f >= value - epsilon) && (f <= value + epsilon); 
}