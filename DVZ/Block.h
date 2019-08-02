#pragma once

namespace Voxel {
	constexpr float BLOCK_RENDER_SIZE = 1.0f;

	typedef enum _BlockType : unsigned char{
		BLOCK_TYPE_DEFAULT = 0,
		BLOCK_TYPE_STONE,
		BLOCK_TYPE_DIRT,
		BLOCK_TYPE_GRASS,
		BLOCK_TYPE_NUM_TYPES
	} BlockType;

	struct Block {
		BlockType type;
		bool isAlive;
	};

	struct BlockVertex {
		float x, y, z;
		float nx, ny, nz;
		float r, g, b;
	};
}