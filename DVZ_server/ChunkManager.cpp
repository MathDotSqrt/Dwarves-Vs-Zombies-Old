#include "ChunkManager.h"
#include "Chunk.h"
#include "EntityAdmin.h"
using namespace Voxel;

#define CHUNK_ALLOC_SIZE 9 * 1024 * 1024

ChunkManager::ChunkManager(Util::Allocator::IAllocator &parent) : 
	voxel_allocator(sizeof(FlatVoxelContainer), alignof(FlatVoxelContainer), CHUNK_ALLOC_SIZE, parent) {

	world.reserve(sizeof(Chunk) * WORLD_X_LENGTH * WORLD_Z_LENGTH);

	for (int z = MIN_CHUNK_Z; z <= MAX_CHUNK_Z; z++) {
		for (int x = MIN_CHUNK_X; x <= MAX_CHUNK_X; x++) {
			world.push_back(Chunk(x, 0, z, *this));
		}
	}
}


ChunkManager::~ChunkManager() {

}

void ChunkManager::update(EntityAdmin &admin, float delta) {
	
}

Chunk& ChunkManager::getChunk(int cx, int cz) {
	int col = cx + abs(MIN_CHUNK_X);
	int row = cz + abs(MIN_CHUNK_Z);
	int index = col + row * WORLD_X_LENGTH;
	return world[index];
}