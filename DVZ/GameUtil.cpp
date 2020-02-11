#include "GameUtil.h"
#include "Engine.h"
#include "ChunkManager.h"
#include "Components.h"

glm::i32vec3 GameUtil::world_to_block_coord(glm::vec3 vec) {
	return glm::i32vec3(glm::floor(vec));
}

bool GameUtil::attempt_place_block_net(Engine &engine, glm::i32vec3 block_coord, Voxel::Block block){
	using namespace Component;
	
	auto view = engine.view<Position, VoxelCollision>();

	for (auto e : view) {
		const auto pos = view.get<Position>(e);
		const auto coll = view.get<VoxelCollision>(e);

		const auto min = pos + coll.aabb.getMin();
		const auto max = pos + coll.aabb.getMax();

		const auto min_block = world_to_block_coord(min);
		const auto max_block = world_to_block_coord(max);

		if (min_block.x <= block_coord.x && block_coord.x <= max_block.x 
			&& min_block.y <= block_coord.y && block_coord.y <= max_block.y
			&& min_block.z <= block_coord.z && block_coord.z <= max_block.z)
			return false;
	}

	auto &manager = engine.ctx<Voxel::ChunkManager>();
	manager.setBlock(block_coord, block);
	engine.getBlockPlaceBuffer().push_back(std::pair(block_coord, block));

	return true;
}