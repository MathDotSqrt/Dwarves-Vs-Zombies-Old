#pragma once

#include "../DVZ_common/BlockTypeEnum.h"

namespace Voxel {
	class Block {
	public:
		BlockType type;

		Block() {}
		Block(BlockType type) : type(type) {}
	};
}