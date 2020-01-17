#include "RLEncoder.h"
#include "Chunk.h"
#include <limits>
using namespace Voxel;

const RLEncoding Voxel::encode_chunk(const Chunk &chunk) {
	constexpr auto MAX_COUNT = std::numeric_limits<RLFreqType>::max();
	
	Voxel::RLEncoding encoding;
	

	for (int i = 0; i < Chunk::MAX_INDEX - 1; i++) {
		RLFreqType count = 1;

		while (count != MAX_COUNT && chunk.getBlock(i) == chunk.getBlock(i + 1)) {
			count++;
			i++;
		}
		
		encoding.push_back(std::make_pair(count, chunk.getBlock(i)));
	}

	return encoding;
}