#pragma once

#include "Engine.h"
#include "Common.h"
#include "glm.hpp"
#include "Block.h"

namespace Voxel {
	class ChunkManager;
}

namespace GameUtil {
	
	entt::entity spawn_client_spunk(Engine &engine, glm::vec3 pos);
	entt::entity spawn_net_spunk(Engine &engine, glm::vec3 pos, glm::quat quat);
	
	glm::i32vec3 world_to_block_coord(glm::vec3 vec3);
	bool attempt_place_block_net(Engine &engine, glm::i32vec3 block_coord, Voxel::Block block);


}
