#pragma once
#include "IntersectionTests.h"

bool Physics::intersect(const Physics::AABB &a, const Physics::AABB &b) {
	return !(
		a.max.x < b.min.x || a.min.x > b.max.x 
		|| a.max.y < b.min.y || a.min.y > b.max.x
		|| a.max.z < b.min.z || a.min.z > b.max.z
	);
}

bool Physics::intersect(const Physics::BoundingCylinder &cylinder, const glm::vec3 &point) {
	return distance_point_ray(point, cylinder.min, cylinder.max) < cylinder.radius;
}

bool Physics::intersect(const Physics::BoundingCylinder &cylinder, const Physics::AABB &aabb) {
	for (int i = 0; i < 8; i++) {
		if (intersect(cylinder, aabb.getPoint(i))) {
			return true;
		}
	}

	return false;
}

glm::vec3 
Physics::project_point_ray(const glm::vec3 &point, const glm::vec3 &start, const glm::vec3 end) {
	const auto rhs = point - start;
	const auto lhs = end - start;
	
	const auto magnitude = glm::length(lhs);
	assert(magnitude >= 1E-06);

	const auto norm_lhs = lhs * (1 / magnitude);
	const float dot = glm::dot(norm_lhs, rhs);
	const float num2 = glm::clamp(dot, 0.0f, magnitude);
	
	return start + (norm_lhs * num2);
}

float Physics::distance_point_ray(const glm::vec3 &point, const glm::vec3 &start, const glm::vec3 &end) {
	return glm::distance(project_point_ray(point, start, end), point);
}