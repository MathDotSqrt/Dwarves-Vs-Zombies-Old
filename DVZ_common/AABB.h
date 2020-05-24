#pragma once
#include "Common.h"
#include "glm.hpp"
#include <assert.h>

namespace Physics {
	struct AABB {
		glm::vec3 min;
		glm::vec3 max;
		

		AABB(glm::vec3 min, glm::vec3 max);

		const glm::vec3 &getMin() const;

		const glm::vec3 &getMax() const;

		glm::vec3 getMiddle() const;

		glm::vec3 getPoint(int i) const;
	};


	AABB translate_aabb(const AABB &aabb, glm::vec3 translate);
}