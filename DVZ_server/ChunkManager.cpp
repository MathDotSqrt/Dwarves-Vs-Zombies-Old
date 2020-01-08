#include "ChunkManager.h"
#include "Chunk.h"

using namespace Voxel;

#define CHUNK_ALLOC_SIZE 9 * 1024 * 1024

ChunkManager::ChunkManager(Util::Allocator::IAllocator &parent) : chunk_allocator(sizeof(Chunk), alignof(Chunk), CHUNK_ALLOC_SIZE, parent){

}


ChunkManager::~ChunkManager() {

}
