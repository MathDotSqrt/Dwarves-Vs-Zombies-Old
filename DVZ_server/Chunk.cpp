#include "Chunk.h"
#include "ChunkManager.h"

using namespace Voxel;

Chunk::Chunk(int cx, int cy, int cz, ChunkManager *manager) : 
	cx(cx), 
	cy(cy), 
	cz(cz), 
	manager(manager), 
	flat(nullptr){
	
	auto &allocator = manager->voxel_allocator;
	flat = Util::Allocator::allocateNew<FlatVoxelContainer>(allocator);

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
	printf("container %p\n", flat);
	//generating the chunk counts as a modification that needs to 
	//be synced with clients
	mod_count++;
}


Chunk::~Chunk() {
	Util::Allocator::free<FlatVoxelContainer>(manager->voxel_allocator, flat);
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