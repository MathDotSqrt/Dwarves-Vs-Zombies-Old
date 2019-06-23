#pragma once
#include "Material.h"
#include <glm.hpp>

class BasicMaterial : public Material {
private:
	glm::uvec4 color;

public:
	BasicMaterial();
	~BasicMaterial();
	
	void setColor(float r, float g, float b);
	void setColor(float r, float g, float b, float a);
	void setColor(glm::vec3 color);
	void setColor(glm::vec4 color);
};

