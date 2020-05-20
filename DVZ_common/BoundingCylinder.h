#pragma once
#include "glm.hpp"

namespace Physics {
	struct BoundingCylinder {
		glm::vec3 min;
		glm::vec3 max;
		float radius;

		BoundingCylinder(glm::vec3 min, glm::vec3 max, float radius);

		const glm::vec3 &getMin() const;
		const glm::vec3 &getMax() const;
		float getRadius() const;
	};


};