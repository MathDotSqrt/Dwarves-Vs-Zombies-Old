#pragma once

namespace Voxel {
	enum BlockType {
		BLOCK_TYPE_DEFAULT = 0,
		BLOCK_TYPE_STONE,
		BLOCK_TYPE_DIRT,
		BLOCK_TYPE_GRASS,
		BLOCK_TYPE_NUM_TYPES
	};


	struct Block {
		bool isAlive;
		BlockType type;
	};
}