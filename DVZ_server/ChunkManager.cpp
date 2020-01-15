#include "ChunkManager.h"
#include "Chunk.h"
#include "EntityAdmin.h"
using namespace Voxel;

#define CHUNK_ALLOC_SIZE 450 * 1024 * 1024

ChunkManager::ChunkManager(Util::Allocator::IAllocator &parent) :  voxel_allocator(sizeof(FlatVoxelContainer), alignof(FlatVoxelContainer), CHUNK_ALLOC_SIZE, parent) {

	world.reserve(sizeof(Chunk) * WORLD_X_LENGTH * WORLD_Z_LENGTH);

	for (int z = MIN_CHUNK_Z; z <= MAX_CHUNK_Z; z++) {
		for (int x = MIN_CHUNK_X; x <= MAX_CHUNK_X; x++) {
			world.emplace_back(Chunk(x, 0, z, *this));
		}
	}

	//voxel_allocator = pool;
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

const Chunk& ChunkManager::getChunk(int cx, int cz) const {
	int col = cx + abs(MIN_CHUNK_X);
	int row = cz + abs(MIN_CHUNK_Z);
	int index = col + row * WORLD_X_LENGTH;
	return world[index];
}


void ChunkManager::setBlock(int x, int y, int z, Block block) {
	int cx = x >> CHUNK_SHIFT_X;
	int cy = y >> CHUNK_SHIFT_Y;
	int cz = z >> CHUNK_SHIFT_Z;

	auto &chunk = getChunk(cx, cz);

	int bx = x & CHUNK_BLOCK_POS_MASK_X;
	int by = y & CHUNK_BLOCK_POS_MASK_Y;
	int bz = z & CHUNK_BLOCK_POS_MASK_Z;
	chunk.setBlock(bx, by, bz, block);
}

Block ChunkManager::getBlock(int x, int y, int z) const {
	int cx = x >> CHUNK_SHIFT_X;
	int cy = y >> CHUNK_SHIFT_Y;
	int cz = z >> CHUNK_SHIFT_Z;

	const auto &chunk = getChunk(cx, cz);

	int bx = x & CHUNK_BLOCK_POS_MASK_X;
	int by = y & CHUNK_BLOCK_POS_MASK_Y;
	int bz = z & CHUNK_BLOCK_POS_MASK_Z;

	return chunk.getBlock(bx, by, bz);
}

Voxel::FlatVoxelContainer* ChunkManager::allocFlatVoxel() {
	return Util::Allocator::allocateNew<FlatVoxelContainer>(voxel_allocator);
}

void ChunkManager::freeFlatVoxel(FlatVoxelContainer *ptr) {
	Util::Allocator::free<FlatVoxelContainer>(voxel_allocator, ptr);
}