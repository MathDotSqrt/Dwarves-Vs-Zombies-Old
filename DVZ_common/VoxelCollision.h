#pragma once

#include "glm.hpp"
#include "Block.h"
#include "AABB.h"
#include <functional>
#include "SharedComponents.h"

namespace Physics {
	typedef std::function<Voxel::Block(glm::i32vec3)> GetBlockFunc;
	
	std::pair<glm::vec3, Component::VoxelCollisionSample> 
	face_collision_handling(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta, GetBlockFunc &func);

	std::pair<glm::vec3, Component::VoxelCollisionSample> 
	edge_collision_handling(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta, GetBlockFunc &func);

	std::pair<glm::vec3, Component::VoxelCollisionSample> 
	corner_collision_handling(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta, GetBlockFunc &func);

}
