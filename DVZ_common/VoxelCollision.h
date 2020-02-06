#pragma once

#include "glm.hpp"
#include "Block.h"
#include "AABB.h"
#include <functional>

namespace Physics {
	typedef std::function<Voxel::Block(glm::i32vec3)> GetBlockFunc;
	
	glm::vec3 face_collision_handling(glm::vec3 pos, glm::vec3 vel, Physics::AABB aabb, float delta, GetBlockFunc &func);
	glm::vec3 edge_collision_handling(glm::vec3 pos, glm::vec3 vel, Physics::AABB aabb, float delta, GetBlockFunc &func);
	glm::vec3 corner_collision_handling(glm::vec3 pos, glm::vec3 vel, Physics::AABB aabb, float delta, GetBlockFunc &func);

}
