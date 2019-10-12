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
	{
		std::shared_lock<std::shared_mutex> middle_lock(n.middle->chunkMutex);
		for (int z = 0; z < CHUNK_WIDTH_Z; z++) { 
			for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
				for (int x = 0; x < CHUNK_WIDTH_X; x++) {
					this->block[toPaddedBlockIndex(x+1, y+1, z+1)] = n.middle->getBlockInternal(x, y, z);
				}
			}
		}
	}
}

void ChunkMesher::createChunkMesh(Chunk::BlockGeometry &geometry) {
	geometry.clear();
	for (int z = 0; z < CHUNK_WIDTH_Z; z++) {
		for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
			for (int x = 0; x < CHUNK_WIDTH_X; x++) {
				Block &block = this->getBlock(x + 1, y + 1, z + 1);
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

void ChunkMesher::createCulledCube(int x, int y, int z, ChunkMesher::BlockFaceCullTags tags, Block b, Chunk::BlockGeometry &geometry) {
	typedef Chunk::BlockGeometry::GeometryVertex BlockVertex;
	
	constexpr float HALF_WIDTH = BLOCK_RENDER_SIZE / 2.0f;

	float x_ = x * BLOCK_RENDER_SIZE;
	float y_ = y * BLOCK_RENDER_SIZE;
	float z_ = z * BLOCK_RENDER_SIZE;

	glm::vec3 p0(x_ - HALF_WIDTH, y_ - HALF_WIDTH, z_ + HALF_WIDTH);
	glm::vec3 p1(x_ + HALF_WIDTH, y_ - HALF_WIDTH, z_ + HALF_WIDTH);
	glm::vec3 p2(x_ + HALF_WIDTH, y_ + HALF_WIDTH, z_ + HALF_WIDTH);
	glm::vec3 p3(x_ - HALF_WIDTH, y_ + HALF_WIDTH, z_ + HALF_WIDTH);

	glm::vec3 p4(x_ + HALF_WIDTH, y_ - HALF_WIDTH, z_ - HALF_WIDTH);
	glm::vec3 p5(x_ - HALF_WIDTH, y_ - HALF_WIDTH, z_ - HALF_WIDTH);
	glm::vec3 p6(x_ - HALF_WIDTH, y_ + HALF_WIDTH, z_ - HALF_WIDTH);
	glm::vec3 p7(x_ + HALF_WIDTH, y_ + HALF_WIDTH, z_ - HALF_WIDTH);

	glm::vec3 c0, c1, c2, c3;

	switch (b.type) {
	case BlockType::BLOCK_TYPE_DIRT:
		c0 = glm::vec3(.7f, .4f, .4f);
		c1 = glm::vec3(.7f, .4f, .4f);
		c2 = glm::vec3(.7f, .4f, .4f);
		c3 = glm::vec3(.7f, .4f, .4f);
		break;
	case BlockType::BLOCK_TYPE_STONE:
		c0 = glm::vec3(.3f, .3f, .3f);
		c1 = glm::vec3(.3f, .3f, .3f);
		c2 = glm::vec3(.3f, .3f, .3f);
		c3 = glm::vec3(.3f, .3f, .3f);
	case BlockType::BLOCK_TYPE_GRASS:
		c0 = glm::vec3(0, 1, 0);
		c1 = glm::vec3(0, 1, 0);
		c2 = glm::vec3(0, 1, 0);
		c3 = glm::vec3(0, 1, 0);
		break;
	case BlockType::BLOCK_TYPE_PURPLE:
		c0 = glm::vec3(.7f, 0, 1);
		c1 = glm::vec3(.7f, 0, 1);
		c2 = glm::vec3(.7f, 0, 1);
		c3 = glm::vec3(.7f, 0, 1);
	default:
		c0 = glm::vec3(1, 1, 1);
		c1 = glm::vec3(1, 1, 1);
		c2 = glm::vec3(1, 1, 1);
		c3 = glm::vec3(1, 1, 1);
	}

	BlockVertex v0, v1, v2, v3;

	/*FRONT*/
	if (tags.pz) {
		glm::vec3 n0(0, 0, 1);
		v0 = { p0, n0, c0 };
		v1 = { p1, n0, c1 };
		v2 = { p2, n0, c2 };
		v3 = { p3, n0, c3 };
		createFace(v0, v1, v2, v3, geometry);
	}

	/*BACK*/
	if (tags.nz) {
		glm::vec3 n1(0, 0, -1);
		v0 = { p4, n1, c0 };
		v1 = { p5, n1, c1 };
		v2 = { p6, n1, c2 };
		v3 = { p7, n1, c3 };
		createFace(v0, v1, v2, v3, geometry);
	}

	/*LEFT*/
	if (tags.nx) {
		glm::vec3 n2(-1, 0, 0);
		v0 = { p5, n2, c0 };
		v1 = { p0, n2, c1 };
		v2 = { p3, n2, c2 };
		v3 = { p6, n2, c3 };
		createFace(v0, v1, v2, v3, geometry);
	}

	/*RIGHT*/
	if (tags.px) {
		glm::vec3 n3(1, 0, 0);
		v0 = { p1, n3, c0 };
		v1 = { p4, n3, c1 };
		v2 = { p7, n3, c2 };
		v3 = { p2, n3, c3 };
		createFace(v0, v1, v2, v3, geometry);
	}

	/*TOP*/
	if (tags.py) {
		glm::vec3 n4(0, 1, 0);
		v0 = { p3, n4, c0 };
		v1 = { p2, n4, c1 };
		v2 = { p7, n4, c2 };
		v3 = { p6, n4, c3 };
		createFace(v0, v1, v2, v3, geometry);
	}

	/*BOTTOM*/
	if (tags.ny) {
		glm::vec3 n5(0, -1, 0);
		v0 = { p1, n5, c0 };
		v1 = { p0, n5, c1 };
		v2 = { p5, n5, c2 };
		v3 = { p4, n5, c3 };
		createFace(v0, v1, v2, v3, geometry);
	}
}

void ChunkMesher::createFace(BlockVertex v0, BlockVertex v1, BlockVertex v2, BlockVertex v3, Chunk::BlockGeometry &geometry) {
	geometry.pushVertex(v0);
	geometry.pushVertex(v1);
	geometry.pushVertex(v2);
	geometry.pushVertex(v3);

	////todo figure out if this is correct
	int lastIndex = (int)geometry.getVertexCount();

	geometry.pushTriangle(lastIndex + 0, lastIndex + 1, lastIndex + 2);
	geometry.pushTriangle(lastIndex + 0, lastIndex + 2, lastIndex + 3);
}

Block& ChunkMesher::getBlock(int x, int y, int z) {
	return this->block[toPaddedBlockIndex(x, y, z)];
}

int ChunkMesher::toPaddedBlockIndex(int x, int y, int z) {
	//return x + CHUNK_WIDTH_X * (y + CHUNK_WIDTH_Y * z);
	return x + PADDED_WIDTH_X * (y + PADDED_WIDTH_Y * z);
}