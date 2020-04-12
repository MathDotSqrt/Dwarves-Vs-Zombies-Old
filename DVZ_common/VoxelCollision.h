#pragma once

#include "glm.hpp"
#include "Block.h"
#include "AABB.h"
#include <functional>
#include "SharedComponents.h"

namespace Physics {
	typedef std::function<Voxel::Block(glm::i32vec3)> GetBlockFunc;
	
	Component::VoxelCollisionSample
	face_collision_sample(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta, GetBlockFunc &func);

	Component::VoxelCollisionSample
	edge_collision_sample(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta, GetBlockFunc &func);

	Component::VoxelCollisionSample
	corner_collision_sample(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta, GetBlockFunc &func);

}
