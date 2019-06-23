#include "BasicMaterial.h"



BasicMaterial::BasicMaterial() {
}


BasicMaterial::~BasicMaterial()
{
}

void BasicMaterial::setColor(float r, float g, float b) {
	this->setColor(r, g, b, 1.0f);
}

void BasicMaterial::setColor(float r, float g, float b, float a) {
	this->color.x = r;
	this->color.y = g;
	this->color.z = b;
	this->color.w = a;
}

void BasicMaterial::setColor(glm::vec3 newColor) {
	this->setColor(newColor.x, newColor.y, newColor.z);
}

void BasicMaterial::setColor(glm::vec4 newColor) {
	this->color = newColor;
}