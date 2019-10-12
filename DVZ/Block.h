#pragma once
#include "glm.hpp"

namespace Voxel {
	constexpr float BLOCK_RENDER_SIZE = 1.0f;

	typedef enum _BlockType : unsigned char{
		BLOCK_TYPE_DEFAULT = 0,
		BLOCK_TYPE_STONE,
		BLOCK_TYPE_DIRT,
		BLOCK_TYPE_GRASS,
		BLOCK_TYPE_PURPLE,
		BLOCK_TYPE_NUM_TYPES
	} BlockType;
	struct Block {
		BlockType type;
		Block() {
			this->type = BlockType::BLOCK_TYPE_DEFAULT;
		}

		Block(BlockType type) {
			this->type = type;
		}

		bool operator==(const Block &other) {
			return this->type == other.type;
		}

		bool operator!=(const Block &other) {
			return this->type != other.type;
		}
	};

}