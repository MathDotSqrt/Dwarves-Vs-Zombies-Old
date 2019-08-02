#include "Chunk.h"

using namespace Voxel;

Chunk::Chunk() {
	data = new Block[Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE];
}


Chunk::~Chunk() {
	delete data;
}
