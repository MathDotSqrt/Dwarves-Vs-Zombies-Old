#pragma once
#include "Block.h"
#include "VAO.h"
#include "VBO.h"
#include <vector>
#include "Geometry.h"
#include "Attrib.h"

namespace Voxel{

static constexpr int CHUNK_SHIFT_X = 4;
static constexpr int CHUNK_SHIFT_Y = 8;
static constexpr int CHUNK_SHIFT_Z = 4;

static constexpr int CHUNK_WIDTH_X = 1 << CHUNK_SHIFT_X;
static constexpr int CHUNK_WIDTH_Y = 1 << CHUNK_SHIFT_Y;
static constexpr int CHUNK_WIDTH_Z = 1 << CHUNK_SHIFT_Z;

static constexpr int CHUNK_BLOCK_POS_MASK_X = CHUNK_WIDTH_X - 1;
static constexpr int CHUNK_BLOCK_POS_MASK_Y = CHUNK_WIDTH_Y - 1;
static constexpr int CHUNK_BLOCK_POS_MASK_Z = CHUNK_WIDTH_Z - 1;

static constexpr int CHUNK_VOLUME = CHUNK_WIDTH_X * CHUNK_WIDTH_Y * CHUNK_WIDTH_Z;

static constexpr float CHUNK_RENDER_WIDTH_X = CHUNK_WIDTH_X * BLOCK_RENDER_SIZE;
static constexpr float CHUNK_RENDER_WIDTH_Y = CHUNK_WIDTH_Y * BLOCK_RENDER_SIZE;
static constexpr float CHUNK_RENDER_WIDTH_Z = CHUNK_WIDTH_Z * BLOCK_RENDER_SIZE;

class Chunk {
private:
	
	typedef Graphics::Geometry<Graphics::PositionAttrib, Graphics::NormalAttrib, Graphics::ColorAttrib> BlockGeometry;
	typedef BlockGeometry::GeometryVertex BlockVertex;

	typedef struct _BlockFaceCullTags {
		bool px;
		bool nx;
		bool py;
		bool ny;
		bool pz;
		bool nz;
	} BlockFaceCullTags;

	const int chunk_x, chunk_y, chunk_z;
	
	Block *data;
	bool isMeshValid;
	bool isEmpty;

	BlockGeometry geometry;
	//std::vector<BlockVertex> verticies;
	//std::vector<GLuint> indices;
	//Graphics::VBO vbo;
	//Graphics::VBO ebo;
	Graphics::VAO vao;
	Graphics::VBO vbo;
	Graphics::VBO ebo;		//TODO: all index buffers are the same. replace this one with a unique one
	size_t indexCount = 0;

public:
	
	Chunk(int x, int y, int z);
	//Chunk(int x, int y, int z, Block *data);
	~Chunk();

	void generateTerrain();
	void generateMesh();
	void bufferDataToGPU();

	Block& getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block &block);
	//void setBlockData(Block *newData);

	inline bool needsMeshUpdate() {
		return this->isMeshValid;
	}

	inline int getChunkX() {
		return this->chunk_x;
	}

	inline int getChunkY() {
		return this->chunk_y;
	}

	inline int getChunkZ() {
		return this->chunk_z;
	}

	inline BlockGeometry& getChunkGeometry() {
		return this->geometry;
	}

	inline Graphics::VAO& getChunkVAO() {
		return this->vao;
	}
private:
	void createCube(int x, int y, int z, BlockFaceCullTags render, BlockType type);
	void createFace(BlockVertex v0, BlockVertex v1, BlockVertex v2, BlockVertex v3);

	int toIndex(int x, int y, int z);
	void assertBlockIndex(int x, int y, int z);
};

}

