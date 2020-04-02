#pragma once

#include "glm.hpp"
#include "Block.h"
#include "AABB.h"
#include <functional>
#include "SharedComponents.h"

namespace Physics {
	typedef std::function<Voxel::Block(glm::i32vec3)> GetBlockFunc;
	
	Component::VoxelCollisionSample face_block_sample(glm::vec3 pos, glm::vec3 vel_delta, const Component::VoxelCollision &collision, GetBlockFunc &func);
	glm::vec3 face_collision_handling(glm::vec3 pos, glm::vec3 vel, Physics::AABB aabb, float delta, GetBlockFunc &func);

	Component::VoxelCollisionSample edge_block_sample(glm::vec3 pos, glm::vec3 vel_delta, const Component::VoxelCollision &collision, GetBlockFunc &func);
	glm::vec3 edge_collision_handling(glm::vec3 pos, glm::vec3 vel, Physics::AABB aabb, float delta, GetBlockFunc &func);

	void corner_block_sample(glm::vec3 pos, glm::vec3 vel_delta, Component::VoxelCollision &collision, GetBlockFunc &func);
	glm::vec3 corner_collision_handling(glm::vec3 pos, glm::vec3 vel, Physics::AABB aabb, float delta, GetBlockFunc &func);

}
