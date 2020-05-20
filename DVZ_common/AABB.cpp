#include "AABB.h"

Physics::AABB::AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max) {};

const glm::vec3 &Physics::AABB::getMin() const {
	return min;
}

const glm::vec3 &Physics::AABB::getMax() const {
	return max;
}

glm::vec3 Physics::AABB::getMiddle() const {
	return (max + min) * .5f;
}

glm::vec3 Physics::AABB::getPoint(int i) const {
	assert(i >= 0 && i < 8);

	switch (i) {
	case 0: return glm::vec3(min.x, min.y, min.z); break;
	case 1: return glm::vec3(max.x, min.y, min.z); break;
	case 2: return glm::vec3(min.x, min.y, max.z); break;
	case 3: return glm::vec3(max.x, min.y, max.z); break;
	case 4: return glm::vec3(min.x, max.y, min.z); break;
	case 5: return glm::vec3(max.x, max.y, min.z); break;
	case 6: return glm::vec3(min.x, max.y, max.z); break;
	case 7: return glm::vec3(max.x, max.y, max.z); break;
	default: assert(false); return glm::vec3(10000000);
	}
}