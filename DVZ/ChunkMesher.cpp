#include "ChunkMesher.h"
#include "ChunkManager.h"
using namespace Voxel;

ChunkMesher::ChunkMesher() {

}


ChunkMesher::~ChunkMesher() {

}

void ChunkMesher::loadChunkData(ChunkNeighbors &n) {

}

void ChunkMesher::loadChunkDataAsync(ChunkNeighbors &n) {
	memset(this->block, BlockType::BLOCK_TYPE_DEFAULT, sizeof(block));
	
	if(n.middle){
		std::shared_lock<std::shared_mutex> middle_lock(n.middle->chunkMutex);
		for (int z = 0; z < CHUNK_WIDTH_Z; z++) { 
			for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
				for (int x = 0; x < CHUNK_WIDTH_X; x++) {
					this->block[toPaddedBlockIndex(x+1, y+1, z+1)] = n.middle->getBlockInternal(x, y, z);
				}
			}
		}
		n.middle->currentState = Chunk::ChunkState::VALID;
	}

	if(n.front && !n.front->isEmpty()){	
		std::shared_lock<std::shared_mutex> front_lock(n.front->chunkMutex);
		for(int y = 0; y < CHUNK_WIDTH_Y; y++){
			for (int x = 0; x < CHUNK_WIDTH_X; x++) {
				this->block[toPaddedBlockIndex(x + 1, y + 1, PADDED_WIDTH_Z-1)] = n.front->getBlockInternal(x, y, 0);
			}
		}
	}

	if (n.back && !n.back->isEmpty()) {
		std::shared_lock<std::shared_mutex> back_lock(n.back->chunkMutex);
		for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
			for (int x = 0; x < CHUNK_WIDTH_X; x++) {
				this->block[toPaddedBlockIndex(x + 1, y + 1, 0)] = n.back->getBlockInternal(x, y, CHUNK_WIDTH_Z-1);
			}
		}
	}

	if (n.left && !n.left->isEmpty()) {
		std::shared_lock<std::shared_mutex> left_lock(n.left->chunkMutex);
		for (int z = 0; z < CHUNK_WIDTH_Z; z++) {
			for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
				this->block[toPaddedBlockIndex(0, y + 1, z+1)] = n.left->getBlockInternal(CHUNK_WIDTH_X - 1, y, z);
			}
		}
	}

	if (n.right && !n.right->isEmpty()) {
		std::shared_lock<std::shared_mutex> right_lock(n.right->chunkMutex);
		for (int z = 0; z < CHUNK_WIDTH_Z; z++) {
			for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
				this->block[toPaddedBlockIndex(PADDED_WIDTH_X-1, y + 1, z + 1)] = n.right->getBlockInternal(0, y, z);
			}
		}
	}
}

void ChunkMesher::createChunkMesh(ChunkGeometry *geometry) {
	geometry->clear();
	for (int z = 0; z < CHUNK_WIDTH_Z; z++) {
		for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
			for (int x = 0; x < CHUNK_WIDTH_X; x++) {
				Block block = this->getBlock(x, y, z);
				if (block.type == BlockType::BLOCK_TYPE_DEFAULT) {
					continue;
				}
				BlockFaceCullTags tags = { true, true, true, true, true, true };

				tags.nx = this->getBlock(x - 1, y, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				tags.px = this->getBlock(x + 1, y, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				tags.ny = this->getBlock(x, y - 1, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				tags.py = this->getBlock(x, y + 1, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				tags.nz = this->getBlock(x, y, z - 1).type == BlockType::BLOCK_TYPE_DEFAULT;
				tags.pz = this->getBlock(x, y, z + 1).type == BlockType::BLOCK_TYPE_DEFAULT;

				this->createCulledCube(x, y, z, tags, block, geometry);
			}
		}
	}
}

void ChunkMesher::createCulledCube(int x, int y, int z, ChunkMesher::BlockFaceCullTags tags, Block b, ChunkGeometry *geometry) {
	constexpr unsigned int BLOCK_WIDTH = 1;

	int x_ = x;
	int y_ = y;
	int z_ = z;

	glm::u8vec3 p0(x_, y_, z_ + BLOCK_WIDTH);
	glm::u8vec3 p1(x_ + BLOCK_WIDTH, y_, z_ + BLOCK_WIDTH);
	glm::u8vec3 p2(x_ + BLOCK_WIDTH, y_ + BLOCK_WIDTH, z_ + BLOCK_WIDTH);
	glm::u8vec3 p3(x_, y_ + BLOCK_WIDTH, z_ + BLOCK_WIDTH);

	glm::u8vec3 p4(x_ + BLOCK_WIDTH, y_, z_);
	glm::u8vec3 p5(x_, y_, z_);
	glm::u8vec3 p6(x_, y_ + BLOCK_WIDTH, z_ );
	glm::u8vec3 p7(x_ + BLOCK_WIDTH, y_ + BLOCK_WIDTH, z_);

	glm::u8vec3 c0, c1, c2, c3;

	switch (b.type) {
	case BlockType::BLOCK_TYPE_DIRT:
		c0 = glm::u8vec3(179, 102, 102);
		c1 = glm::u8vec3(179, 102, 102);
		c2 = glm::u8vec3(179, 102, 102);
		c3 = glm::u8vec3(179, 102, 102);
		break;
	case BlockType::BLOCK_TYPE_STONE:
		c0 = glm::u8vec3(77, 77, 77);
		c1 = glm::u8vec3(77, 77, 77);
		c2 = glm::u8vec3(77, 77, 77);
		c3 = glm::u8vec3(77, 77, 77);
		break;
	case BlockType::BLOCK_TYPE_GRASS:
		c0 = glm::u8vec3(0, 255, 0);
		c1 = glm::u8vec3(0, 255, 0);
		c2 = glm::u8vec3(0, 255, 0);
		c3 = glm::u8vec3(0, 255, 0);
		break;
	case BlockType::BLOCK_TYPE_PURPLE:
		c0 = glm::u8vec3(179, 0, 255);
		c1 = glm::u8vec3(179, 0, 255);
		c2 = glm::u8vec3(179, 0, 255);
		c3 = glm::u8vec3(179, 0, 255);
		break;
	default:
		c0 = glm::u8vec3(255, 255, 255);
		c1 = glm::u8vec3(255, 255, 255);
		c2 = glm::u8vec3(255, 255, 255);
		c3 = glm::u8vec3(255, 255, 255);
		break;
	}

	BlockVertex v0, v1, v2, v3;

	/*FRONT*/
	if (tags.pz) {
		glm::i8vec3 n0(0, 0, 1);
		v0 = { p0, n0, c0 };
		v1 = { p1, n0, c1 };
		v2 = { p2, n0, c2 };
		v3 = { p3, n0, c3 };
		geometry->appendFace(v0, v1, v2, v3);
	}

	/*BACK*/
	if (tags.nz) {
		glm::i8vec3 n1(0, 0, -1);
		v0 = { p4, n1, c0 };
		v1 = { p5, n1, c1 };
		v2 = { p6, n1, c2 };
		v3 = { p7, n1, c3 };
		geometry->appendFace(v0, v1, v2, v3);
	}

	/*LEFT*/
	if (tags.nx) {
		glm::i8vec3 n2(-1, 0, 0);
		v0 = { p5, n2, c0 };
		v1 = { p0, n2, c1 };
		v2 = { p3, n2, c2 };
		v3 = { p6, n2, c3 };
		geometry->appendFace(v0, v1, v2, v3);
	}

	/*RIGHT*/
	if (tags.px) {
		glm::i8vec3 n3(1, 0, 0);
		v0 = { p1, n3, c0 };
		v1 = { p4, n3, c1 };
		v2 = { p7, n3, c2 };
		v3 = { p2, n3, c3 };
		geometry->appendFace(v0, v1, v2, v3);
	}

	/*TOP*/
	if (tags.py) {
		glm::i8vec3 n4(0, 1, 0);
		v0 = { p3, n4, c0 };
		v1 = { p2, n4, c1 };
		v2 = { p7, n4, c2 };
		v3 = { p6, n4, c3 };
		geometry->appendFace(v0, v1, v2, v3);
	}

	/*BOTTOM*/
	if (tags.ny) {
		glm::i8vec3 n5(0, -1, 0);
		v0 = { p1, n5, c0 };
		v1 = { p0, n5, c1 };
		v2 = { p5, n5, c2 };
		v3 = { p4, n5, c3 };
		geometry->appendFace(v0, v1, v2, v3);
	}
}

Block ChunkMesher::getBlock(int x, int y, int z) {
	return this->block[toPaddedBlockIndex(x+1, y+1, z+1)];
}

int ChunkMesher::toPaddedBlockIndex(int x, int y, int z) {
	//return x + CHUNK_WIDTH_X * (y + CHUNK_WIDTH_Y * z);
	return x + PADDED_WIDTH_X * (y + PADDED_WIDTH_Y * z);
}