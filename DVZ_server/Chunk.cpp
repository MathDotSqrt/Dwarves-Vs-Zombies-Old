#include "Chunk.h"
#include "ChunkManager.h"

using namespace Voxel;

Chunk::Chunk(int cx, int cy, int cz, ChunkManager &manager) : 
	cx(cx), 
	cy(cy), 
	cz(cz), 
	manager(manager), 
	flat(Util::Allocator::allocateNew<FlatVoxelContainer>(manager.voxel_allocator)){


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