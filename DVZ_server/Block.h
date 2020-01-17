#pragma once

#include "BlockTypeEnum.h"

namespace Voxel {
	class Block {
	public:
		BlockType type;

		Block() {}
		Block(BlockType type) : type(type) {}

		bool operator==(const Block &other) const {
			return type == other.type;
		}

		bool operator!=(const Block &other) const {
			return type != other.type;
		}
	};
}