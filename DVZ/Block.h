#pragma once
#include "glm.hpp"

namespace Voxel {
	constexpr float BLOCK_RENDER_SIZE = .5f;

	typedef enum _BlockType : unsigned char{
		BLOCK_TYPE_DEFAULT = 0,
		BLOCK_TYPE_STONE,
		BLOCK_TYPE_DIRT,
		BLOCK_TYPE_GRASS,
		BLOCK_TYPE_NUM_TYPES
	} BlockType;

	struct Block {
		BlockType type;
	};

	struct BlockVertex {
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec3 col;
	};
}