#pragma once

#include "Common.h"
#include "BlockTypeEnum.h"
#include <vector>

namespace Voxel {
	class Chunk;

	typedef uint8 RLFreqType;
	typedef std::pair<RLFreqType, BlockType> RLElement;
	typedef std::vector<RLElement> RLEncoding;
}