#pragma once

#include "Common.h"
#include "glm.hpp"
#include "Block.h"

class Engine;
namespace Voxel {
	class ChunkManager;
}

namespace GameUtil {
	glm::i32vec3 world_to_block_coord(glm::vec3 vec3);
	bool attempt_place_block_net(Engine &engine, glm::i32vec3 block_coord, Voxel::Block block);


}
