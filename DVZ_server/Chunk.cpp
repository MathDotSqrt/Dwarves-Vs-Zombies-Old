#include "Chunk.h"
#include "ChunkManager.h"
#include "PerlinNoise.h"

using namespace Voxel;

Chunk::Chunk(int cx, int cy, int cz, ChunkManager &manager) :
	cx(cx), 
	cy(cy), 
	cz(cz), 
	manager(manager), 
	flat(Util::Allocator::allocateNew<FlatVoxelContainer>(manager.voxel_allocator)){
	
	static Util::PerlinNoise noise(3);


	//for (int by = 0; by < CHUNK_WIDTH_Y; by++) {
	//	for (int bz = 0; bz < CHUNK_WIDTH_Z; bz++) {
	//		for (int bx = 0; bx < CHUNK_WIDTH_X; bx++) {
	//			int i = toIndex(bx, by, bz);
	//			
	//			if (by <= CHUNK_WIDTH_Y / 3 + (cz + cx) * 1) {
	//				if (by == CHUNK_WIDTH_Y / 3 + (cz + cx) * 1) {
	//					setBlock(i, Block(BlockType::BLOCK_TYPE_GRASS));
	//				}
	//				else {
	//					setBlock(i, Block(BlockType::BLOCK_TYPE_DIRT));
	//				}
	//			}
	//			else {
	//				setBlock(i, Block(BlockType::BLOCK_TYPE_DEFAULT));
	//			}
	//		}
	//	}
	//}

	for (int bz = 0; bz < CHUNK_WIDTH_Z; bz++) {
		for (int bx = 0; bx < CHUNK_WIDTH_X; bx++) {
			int x = this->cx * CHUNK_WIDTH_X + bx;
			int z = this->cz * CHUNK_WIDTH_Z + bz;

			//if ((chunk_x + chunk_z) % 2 == 0) {
			//	setBlockInternal(bx, 0, bz, Block(BlockType::BLOCK_TYPE_PURPLE));
			//}
			//else {
			//	setBlockInternal(bx, 0, bz, Block(BlockType::BLOCK_TYPE_DIRT));

			//}

			double height = pow(noise.octaveNoise0_1(x / 140.0, z / 140.0, 6), 4.5) * 155;

			for (int by = 0; by < CHUNK_WIDTH_Y; by++) {

				int y = this->cy * CHUNK_WIDTH_Y + by;

				if (y == 0) {
					setBlockInternal(bx, by, bz, Block(BlockType::BLOCK_TYPE_STONE));
				}
				else if (y < height) {

					setBlockInternal(bx, by, bz, Block(BlockType::BLOCK_TYPE_DIRT));

				}
				else if (y < height + 1) {
					double sandValue = noise.octaveNoise0_1(x / 30.0, z / 30.0, 1);
					if ((y < 10) && sandValue > .6) {
						setBlockInternal(bx, by, bz, Block(BlockType::BLOCK_TYPE_SAND));

					}
					else {
						setBlockInternal(bx, by, bz, Block(BlockType::BLOCK_TYPE_GRASS));
						if (noise.noise0_1(x * .15, z * .15, 0) > 0.75) {
							setBlockInternal(bx, by + 1, bz, Block(BlockType::BLOCK_TYPE_ROSE));

						}
					}

				}
				else {
					setBlockInternal(bx, by, bz, Block(BlockType::BLOCK_TYPE_DEFAULT));
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
	setBlockInternal(i, block);
	mod_buffer.push_front(ChunkModEvent(i, block));
	mod_count++;
}

void Chunk::flagEntireChunkModified() {
	mod_buffer.push_front(ChunkModEvent());
	mod_count++;
}

bool Chunk::isChunkModHistoryComplete(size_t mod_delta) const {
	//mod delta is outside of buffer range
	if (mod_delta > mod_buffer.get_size()) {
		return false;
	}
	
	for (int i = 0; i < mod_delta; i++) {
		if (mod_buffer.read(i) == ChunkModEvent()) {
			return false;
		}
	}
	return true;
}

int Chunk::getModCount() const{
	return mod_count;
}

const Util::RingBuffer<ChunkModEvent, Chunk::MOD_BUFFER_SIZE>& Chunk::getModBuffer() const {
	return mod_buffer;
}

void Chunk::setBlockInternal(const int x, const int y, const int z, Block block){
	setBlockInternal(toIndex(x, y, z), block);
}

void Chunk::setBlockInternal(const int i, Block block) { 
	if (flat->blocks[i] != block) {
		flat->blocks[i] = block;
	}
}