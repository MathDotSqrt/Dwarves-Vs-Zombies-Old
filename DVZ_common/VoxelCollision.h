#pragma once

#include "glm.hpp"
#include "Block.h"
#include "AABB.h"
#include <functional>
#include "SharedComponents.h"

namespace Physics {
	typedef std::function<Voxel::Block(glm::i32vec3)> GetBlockFunc;

	std::vector<std::pair<BlockCoord, Voxel::Block>> broadphase(
		const Physics::AABB &worldspace_aabb,
		const glm::vec3 &vel_delta,
		GetBlockFunc &getBlock
	);

	glm::vec3 sample_terrain_collision(
		const glm::vec3 &pos, 
		const glm::vec3 &vel,
		const Physics::AABB &aabb,
		float delta,
		std::vector<std::pair<BlockCoord, Voxel::Block>> &broadphase,
		Component::VoxelCollisionSample &sample
	);
}
