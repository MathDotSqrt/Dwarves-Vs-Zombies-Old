#include "Chunk.h"
#include "ChunkManager.h"

using namespace Voxel;

Chunk::Chunk(int cx, int cy, int cz, ChunkManager &manager) :
	cx(cx), 
	cy(cy), 
	cz(cz), 
	manager(manager), 
	flat(Util::Allocator::allocateNew<FlatVoxelContainer>(manager.voxel_allocator)){
	
	for (int by = 0; by < CHUNK_WIDTH_Y; by++) {
		for (int bz = 0; bz < CHUNK_WIDTH_Z; bz++) {
			for (int bx = 0; bx < CHUNK_WIDTH_X; bx++) {
				int i = toIndex(bx, by, bz);
				
				if (by < CHUNK_WIDTH_Y / 3 + cy * 5) {
					setBlock(i, Block(BlockType::BLOCK_TYPE_DIRT));
				}
				else {
					setBlock(i, Block(BlockType::BLOCK_TYPE_DEFAULT));
				}
			}
		}
	}

	//generating the chunk counts as a modification that needs to 
	//be synced with clients
	mod_count++; 
}

Chunk::Chunk(Chunk &&other) : 
	cx(other.cx), 
	cy(other.cy), 
	cz(other.cz), 
	manager(other.manager), 
	flat(other.flat) {
	
	this->mod_count = other.mod_count;
	other.mod_count = 0;
	other.flat = nullptr;

}

Chunk::~Chunk() {
	Util::Allocator::free(manager.voxel_allocator, flat);
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
	if (flat->blocks[i] != block) {
		flat->blocks[i] = block;
		mod_count++;
	}
}

int Chunk::getModCount() const{
	return mod_count;
}