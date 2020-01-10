#include "Chunk.h"
#include "ChunkManager.h"

using namespace Voxel;

Chunk::Chunk(int cx, int cy, int cz, ChunkManager &manager) : 
	cx(cx), 
	cy(cy), 
	cz(cz), 
	manager(manager), 
	flat(Util::Allocator::allocateNew<FlatVoxelContainer>(manager.voxel_allocator)){

	for (int bz = 0; bz < CHUNK_WIDTH_Z; bz++) {
		for (int by = 0; by < CHUNK_WIDTH_Y; by++) {
			for (int bx = 0; bx < CHUNK_WIDTH_X; bx++) {
				int i = toIndex(bx, by, bz);
				
				if (by < CHUNK_WIDTH_Y / 3) {
					setBlock(i, Block(BlockType::BLOCK_TYPE_DIRT));
				}
				else {
					setBlock(i, Block(BlockType::BLOCK_TYPE_DEFAULT));
				}
			}
		}
	}
}


Chunk::~Chunk() {
	Util::Allocator::free<FlatVoxelContainer>(manager.voxel_allocator, flat);
}

Block Chunk::getBlock(const int x, const int y, const int z) const {
	return getBlock(toIndex(x, y, z));
}

Block Chunk::getBlock(const int i) const {
	return flat->blocks[i];
}

void Chunk::setBlock(const int x, const int y, const int z, Block block) {
	setBlock(toIndex(x, y, z), block);
}

void Chunk::setBlock(const int i, Block block) {
	flat->blocks[i] = block;
}