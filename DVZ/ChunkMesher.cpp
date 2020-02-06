#include "ChunkMesher.h"
#include "ChunkManager.h"
using namespace Voxel;

ChunkMesher::ChunkMesher() {

}


ChunkMesher::~ChunkMesher() {

}

void ChunkMesher::loadChunkData(const ChunkNeighbors &n) {
	memset(this->blockData, BlockType::BLOCK_TYPE_DEFAULT, sizeof(blockData));
	if (n.middle) {
		for (int z = 0; z < CHUNK_WIDTH_Z; z++) {
			for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
				for (int x = 0; x < CHUNK_WIDTH_X; x++) {
					int index = toPaddedBlockIndex(x + 1, y + 1, z + 1);
					this->blockData[index] = n.middle->getBlockInternal(x, y, z);
					this->lightData[index] = n.middle->getLightInternal(x, y, z);
				}
			}
		}

		//memcpy(this->lightData, n.middle->lightData, sizeof(Light) * CHUNK_VOLUME);
	}

	if (n.front && !n.front->isEmpty()) {
		for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
			for (int x = 0; x < CHUNK_WIDTH_X; x++) {
				int index = toPaddedBlockIndex(x + 1, y + 1, PADDED_WIDTH_Z - 1);
				this->blockData[index] = n.front->getBlockInternal(x, y, 0);
				this->lightData[index] = n.front->getLightInternal(x, y, 0);

			}
		}
	}

	if (n.back && !n.back->isEmpty()) {
		for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
			for (int x = 0; x < CHUNK_WIDTH_X; x++) {
				int index = toPaddedBlockIndex(x + 1, y + 1, 0);
				this->blockData[index] = n.back->getBlockInternal(x, y, CHUNK_WIDTH_Z - 1);
				this->lightData[index] = n.back->getLightInternal(x, y, CHUNK_WIDTH_Z - 1);
			}
		}
	}

	if (n.left && !n.left->isEmpty()) {
		for (int z = 0; z < CHUNK_WIDTH_Z; z++) {
			for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
				int index = toPaddedBlockIndex(0, y + 1, z + 1);
				this->blockData[index] = n.left->getBlockInternal(CHUNK_WIDTH_X - 1, y, z);
				this->lightData[index] = n.left->getLightInternal(CHUNK_WIDTH_X - 1, y, z);
			}
		}
	}

	if (n.right && !n.right->isEmpty()) {
		for (int z = 0; z < CHUNK_WIDTH_Z; z++) {
			for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
				int index = toPaddedBlockIndex(PADDED_WIDTH_X - 1, y + 1, z + 1);
				this->blockData[index] = n.right->getBlockInternal(0, y, z);
				this->lightData[index] = n.right->getLightInternal(0, y, z);
			}
		}
	}
}

void ChunkMesher::loadChunkDataAsync(const ChunkNeighbors &n) {
	memset(this->blockData, BlockType::BLOCK_TYPE_DEFAULT, sizeof(blockData));
	
	if(n.middle){
		std::shared_lock<std::shared_mutex> middle_lock(n.middle->chunkMutex);
		for (int z = 0; z < CHUNK_WIDTH_Z; z++) { 
			for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
				for (int x = 0; x < CHUNK_WIDTH_X; x++) {
					int index = toPaddedBlockIndex(x + 1, y + 1, z + 1);
					this->blockData[index] = n.middle->getBlockInternal(x, y, z);
					this->lightData[index] = n.middle->getLightInternal(x, y, z);
				}
			}
		}
		//memcpy(this->lightData, n.middle->lightData, sizeof(Light) * CHUNK_VOLUME);
	}

	if(n.front && !n.front->isEmpty()){	
		std::shared_lock<std::shared_mutex> front_lock(n.front->chunkMutex);
		for(int y = 0; y < CHUNK_WIDTH_Y; y++){
			for (int x = 0; x < CHUNK_WIDTH_X; x++) {
				int index = toPaddedBlockIndex(x + 1, y + 1, PADDED_WIDTH_Z - 1);
				this->blockData[index] = n.front->getBlockInternal(x, y, 0);
				this->lightData[index] = n.front->getLightInternal(x, y, 0);
			}
		}
	}

	if (n.back && !n.back->isEmpty()) {
		std::shared_lock<std::shared_mutex> back_lock(n.back->chunkMutex);
		for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
			for (int x = 0; x < CHUNK_WIDTH_X; x++) {
				int index = toPaddedBlockIndex(x + 1, y + 1, 0);
				this->blockData[index] = n.back->getBlockInternal(x, y, CHUNK_WIDTH_Z - 1);
				this->lightData[index] = n.back->getLightInternal(x, y, CHUNK_WIDTH_Z-1);
			}
		}
	}

	if (n.left && !n.left->isEmpty()) {
		std::shared_lock<std::shared_mutex> left_lock(n.left->chunkMutex);
		for (int z = 0; z < CHUNK_WIDTH_Z; z++) {
			for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
				int index = toPaddedBlockIndex(0, y + 1, z + 1);
				this->blockData[index] = n.left->getBlockInternal(CHUNK_WIDTH_X - 1, y, z);
				this->lightData[index] = n.left->getLightInternal(CHUNK_WIDTH_X - 1, y, z);
			}
		}
	}

	if (n.right && !n.right->isEmpty()) {
		std::shared_lock<std::shared_mutex> right_lock(n.right->chunkMutex);
		for (int z = 0; z < CHUNK_WIDTH_Z; z++) {
			for (int y = 0; y < CHUNK_WIDTH_Y; y++) {
				int index = toPaddedBlockIndex(PADDED_WIDTH_X - 1, y + 1, z + 1);
				this->blockData[index] = n.right->getBlockInternal(0, y, z);
				this->lightData[index] = n.right->getLightInternal(0, y, z);
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
				if (block.getMeshType() == MeshType::MESH_TYPE_NONE) {
					continue;
				}

				//tags.nx = this->getBlock(x - 1, y, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				//tags.px = this->getBlock(x + 1, y, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				//tags.ny = this->getBlock(x, y - 1, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				//tags.py = this->getBlock(x, y + 1, z).type == BlockType::BLOCK_TYPE_DEFAULT;
				//tags.nz = this->getBlock(x, y, z - 1).type == BlockType::BLOCK_TYPE_DEFAULT;
				//tags.pz = this->getBlock(x, y, z + 1).type == BlockType::BLOCK_TYPE_DEFAULT;

				if (block.getMeshType() == MeshType::MESH_TYPE_BLOCK) {
					BlockFaceCullTags tags;
					tags.px = block.isOccludedBy(getBlock(x + 1, y, z));
					tags.nx = block.isOccludedBy(getBlock(x - 1, y, z));
					tags.py = block.isOccludedBy(getBlock(x, y + 1, z));
					tags.ny = block.isOccludedBy(getBlock(x, y - 1, z));
					tags.pz = block.isOccludedBy(getBlock(x, y, z + 1));
					tags.nz = block.isOccludedBy(getBlock(x, y, z - 1));

					LightFace l;
					l.px = getLight(x + 1, y, z);
					l.nx = getLight(x - 1, y, z);
					l.py = getLight(x, y + 1, z);
					l.ny = getLight(x, y - 1, z);
					l.pz = getLight(x, y, z + 1);
					l.nz = getLight(x, y, z - 1);

					createCulledCube(x, y, z, block, l, tags, geometry);
				}
				else {
					Light light = getLight(x, y, z);
					createX(x, y, z, block, light, geometry);
				}
					
			}
		}
	}
}

void ChunkMesher::createCulledCube(int x, int y, int z, Block b, LightFace l, ChunkMesher::BlockFaceCullTags tags, ChunkGeometry *geometry) {
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

	//glm::u8vec3 c0 = b.getColor();

	auto uv = b.getTexCoords();


	/*0---1*/
	/*|   |*/
	/*3---2*/
	BlockVertex v0, v1, v2, v3;

	/*FRONT*/
	if (!tags.pz) {
		glm::i8vec3 n0(0, 0, 1);
		v0 = { glm::u8vec4(p0, l.pz), n0, uv.coord.front.uv2() };
		v1 = { glm::u8vec4(p1, l.pz), n0, uv.coord.front.uv3() };
		v2 = { glm::u8vec4(p2, l.pz), n0, uv.coord.front.uv1() };
		v3 = { glm::u8vec4(p3, l.pz), n0, uv.coord.front.uv0() };
		geometry->appendFace(v0, v1, v2, v3);
	}

	/*BACK*/
	if (!tags.nz) {
		glm::i8vec3 n1(0, 0, -1);
		v0 = { glm::u8vec4(p4, l.nz), n1, uv.coord.back.uv2() };
		v1 = { glm::u8vec4(p5, l.nz), n1, uv.coord.back.uv3() };
		v2 = { glm::u8vec4(p6, l.nz), n1, uv.coord.back.uv1() };
		v3 = { glm::u8vec4(p7, l.nz), n1, uv.coord.back.uv0() };
		geometry->appendFace(v0, v1, v2, v3);
	}

	/*LEFT*/
	if (!tags.nx) {
		glm::i8vec3 n2(-1, 0, 0);
		v0 = { glm::u8vec4(p5, l.nx), n2, uv.coord.left.uv2() };
		v1 = { glm::u8vec4(p0, l.nx), n2, uv.coord.left.uv3() };
		v2 = { glm::u8vec4(p3, l.nx), n2, uv.coord.left.uv1() };
		v3 = { glm::u8vec4(p6, l.nx), n2, uv.coord.left.uv0() };
		geometry->appendFace(v0, v1, v2, v3);
	}

	/*RIGHT*/
	if (!tags.px) {
		glm::i8vec3 n3(1, 0, 0);
		v0 = { glm::u8vec4(p1, l.px), n3, uv.coord.right.uv2() };
		v1 = { glm::u8vec4(p4, l.px), n3, uv.coord.right.uv3() };
		v2 = { glm::u8vec4(p7, l.px), n3, uv.coord.right.uv1() };
		v3 = { glm::u8vec4(p2, l.px), n3, uv.coord.right.uv0() };
		geometry->appendFace(v0, v1, v2, v3);
	}

	/*TOP*/
	if (!tags.py) {
		glm::i8vec3 n4(0, 1, 0);
		v0 = { glm::u8vec4(p3, l.py), n4, uv.coord.top.uv2() };
		v1 = { glm::u8vec4(p2, l.py), n4, uv.coord.top.uv3() };
		v2 = { glm::u8vec4(p7, l.py), n4, uv.coord.top.uv1() };
		v3 = { glm::u8vec4(p6, l.py), n4, uv.coord.top.uv0() };
		geometry->appendFace(v0, v1, v2, v3);
	}

	/*BOTTOM*/
	if (!tags.ny) {
		glm::i8vec3 n5(0, -1, 0);
		v0 = { glm::u8vec4(p1, l.ny), n5, uv.coord.bottom.uv2() };
		v1 = { glm::u8vec4(p0, l.ny), n5, uv.coord.bottom.uv3() };
		v2 = { glm::u8vec4(p5, l.ny), n5, uv.coord.bottom.uv1() };
		v3 = { glm::u8vec4(p4, l.ny), n5, uv.coord.bottom.uv0() };
		geometry->appendFace(v0, v1, v2, v3);
	}
}

void ChunkMesher::createX(int x, int y, int z, Block b, Light l, ChunkGeometry *geometry) { 
	constexpr unsigned int BLOCK_WIDTH = 1;

	int x_ = x;
	int y_ = y;
	int z_ = z;

	auto value = l.value;
	glm::u8vec4 p0(x_, y_, z_ + BLOCK_WIDTH, value);
	glm::u8vec4 p1(x_ + BLOCK_WIDTH, y_, z_ + BLOCK_WIDTH, value);
	glm::u8vec4 p2(x_ + BLOCK_WIDTH, y_ + BLOCK_WIDTH, z_ + BLOCK_WIDTH, value);
	glm::u8vec4 p3(x_, y_ + BLOCK_WIDTH, z_ + BLOCK_WIDTH, value);

	glm::u8vec4 p4(x_ + BLOCK_WIDTH, y_, z_, value);
	glm::u8vec4 p5(x_, y_, z_, value);
	glm::u8vec4 p6(x_, y_ + BLOCK_WIDTH, z_, value);
	glm::u8vec4 p7(x_ + BLOCK_WIDTH, y_ + BLOCK_WIDTH, z_, value);

	auto uv = b.getTexCoords();

	/*0---1*/
	/*|   |*/
	/*3---2*/
	BlockVertex v0, v1, v2, v3;

	glm::i8vec3 n0(0, 0, 0);
	v0 = { p0, n0, uv.coord.front.uv2() };
	v1 = { p4, n0, uv.coord.front.uv3() };
	v2 = { p7, n0, uv.coord.front.uv1() };
	v3 = { p3, n0, uv.coord.front.uv0() };
	geometry->appendFace(v0, v1, v2, v3);
	geometry->appendFace(v1, v0, v3, v2);

	v0 = { p1, n0, uv.coord.front.uv2() };
	v1 = { p5, n0, uv.coord.front.uv3() };
	v2 = { p6, n0, uv.coord.front.uv1() };
	v3 = { p2, n0, uv.coord.front.uv0() };
	geometry->appendFace(v0, v1, v2, v3);
	geometry->appendFace(v1, v0, v3, v2);
}

Block ChunkMesher::getBlock(int x, int y, int z) {
	return this->blockData[toPaddedBlockIndex(x+1, y+1, z+1)];
}

Light ChunkMesher::getLight(int x, int y, int z) {
	bool bx = x < -1 || x > CHUNK_WIDTH_X;
	bool by = y < -1 || y > CHUNK_WIDTH_Y;
	bool bz = z < -1 || z > CHUNK_WIDTH_Z;

	if (bx || by || bz) {
		return Light();
	}

	return lightData[toPaddedBlockIndex(x+1, y+1, z+1)];
}

int ChunkMesher::toPaddedBlockIndex(int x, int y, int z) {
	//return x + CHUNK_WIDTH_X * (y + CHUNK_WIDTH_Y * z);
	return x + PADDED_WIDTH_X * (y + PADDED_WIDTH_Y * z);
}