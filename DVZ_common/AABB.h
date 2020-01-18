#pragma once
#include "Common.h"
#include "glm.hpp"

namespace Physics {
	struct AABB {
		const float32 min_x, min_y, min_z;
		const float32 width, height, length;

		AABB( float min_x, float min_y, float min_z, 
			  float width, float height, float length) : 
			min_x(min_x), min_y(min_y), min_z(min_z),
			width(width), height(height), length(length){}

		constexpr float32 getMinX() const {
			return min_x;
		}

		constexpr float32 getMinY() const {
			return min_y;
		}

		constexpr float32 getMinZ() const {
			return min_z;
		}

		constexpr float32 getMaxX() const {
			return min_x + width;
		}

		constexpr float32 getMaxY() const {
			return min_y + height;
		}

		constexpr float32 getMaxZ() const {
			return min_z + length;
		}
	};
}