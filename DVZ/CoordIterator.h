#pragma once
#include "glm.hpp"

namespace Voxel {
	class CoordIterator{
	private:
		const glm::i32vec3 min, max;
		glm::i32vec3 next_coord;
	public:
		CoordIterator(glm::i32vec3 min, glm::i32vec3 max);
		~CoordIterator();

		glm::i32vec3 next();
		constexpr int length() const;
	};
}

