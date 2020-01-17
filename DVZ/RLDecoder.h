#pragma once
#include "Common.h"
#include "Block.h"
#include <vector>


namespace Voxel {
	typedef uint16 RLFreqType;
	typedef std::pair<RLFreqType, Block> RLElement;
	typedef std::vector<RLElement> RLEncoding;

	
}