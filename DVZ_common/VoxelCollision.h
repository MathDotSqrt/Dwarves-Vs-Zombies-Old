#pragma once

#include "glm.hpp"
#include "Block.h"
#include "AABB.h"
#include <functional>
#include "SharedComponents.h"

namespace Physics {
	typedef std::function<Voxel::Block(glm::i32vec3)> GetBlockFunc;
	
	glm::vec3 sample_terrain_collision(
		const glm::vec3 &pos, 
		const glm::vec3 &vel,
		const Physics::AABB &aabb,
		Component::VoxelCollisionSample &sample,
		float delta,
		GetBlockFunc &getBlock
	);
}
