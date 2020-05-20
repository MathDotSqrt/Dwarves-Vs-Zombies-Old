#pragma once

#include "AABB.h"
#include "BoundingCylinder.h"

namespace Physics {
	bool intersect(const AABB &a, const AABB &b);


	bool intersect(const BoundingCylinder &cylinder, const glm::vec3 &point);
	bool intersect(const BoundingCylinder &cylinder, const AABB &aabb);

	glm::vec3 project_point_ray(const glm::vec3 &point, const glm::vec3 &start, const glm::vec3 end);
	float distance_point_ray(const glm::vec3 &point, const glm::vec3 &start, const glm::vec3 &end);
}