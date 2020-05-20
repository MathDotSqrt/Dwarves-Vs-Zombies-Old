#pragma once
#include "BoundingCylinder.h"

Physics::BoundingCylinder::BoundingCylinder(glm::vec3 min, glm::vec3 max, float radius) :
	min(min), 
	max(max), 
	radius(radius){
}

const glm::vec3 &Physics::BoundingCylinder::getMin() const {
	return min;
}

const glm::vec3 &Physics::BoundingCylinder::getMax() const {
	return max;
}

float Physics::BoundingCylinder::getRadius() const {
	return radius;
}