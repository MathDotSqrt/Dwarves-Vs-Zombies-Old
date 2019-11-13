#pragma once
#include "Block.h"
#include <shared_mutex>

namespace Voxel{

static constexpr int CHUNK_SHIFT_X = 5;
static constexpr int CHUNK_SHIFT_Y = 7;
static constexpr int CHUNK_SHIFT_Z = 5;

static constexpr int CHUNK_WIDTH_X = 1 << CHUNK_SHIFT_X;
static constexpr int CHUNK_WIDTH_Y = 1 << CHUNK_SHIFT_Y;
static constexpr int CHUNK_WIDTH_Z = 1 << CHUNK_SHIFT_Z;

static constexpr int CHUNK_BLOCK_POS_MASK_X = CHUNK_WIDTH_X - 1;
static constexpr int CHUNK_BLOCK_POS_MASK_Y = CHUNK_WIDTH_Y - 1;
static constexpr int CHUNK_BLOCK_POS_MASK_Z = CHUNK_WIDTH_Z - 1;

static constexpr int CHUNK_VOLUME = CHUNK_WIDTH_X * CHUNK_WIDTH_Y * CHUNK_WIDTH_Z;

static constexpr float CHUNK_RENDER_WIDTH_X = CHUNK_WIDTH_X;
static constexpr float CHUNK_RENDER_WIDTH_Y = CHUNK_WIDTH_Y;
static constexpr float CHUNK_RENDER_WIDTH_Z = CHUNK_WIDTH_Z;

enum class BlockState {
	NONE,
	LOADED,
	LOADED_AND_EMPTY,
	LOCKED,
	NUM_BLOCK_STATES
};

enum class MeshState {
	NONE_MESH,
	VALID,
	DIRTY,
	LOCKED,
	NUM_MESH_STATES
};

//DO NOT CALL THIS ON STACK
class Chunk {

private:
	friend class ChunkMesher;
	friend class ChunkManager;

	int chunk_x, chunk_y, chunk_z;
	
	Block data[CHUNK_VOLUME];
	BlockState blockState;
	MeshState meshState;

	std::shared_mutex chunkMutex;

public:
	
	//todo make chunk contain shared pointer of render data from a recycler. Maybe
	Chunk(int x, int y, int z);
	~Chunk();

	void generateTerrain();

	void flagMeshValid();
	
	bool isEmpty();
	
	BlockState getBlockState();
	MeshState getMeshState();

	BlockState tryGetBlockState();
	MeshState tryGetMeshState();

	Block getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block block);

	inline int getChunkX() {
		return this->chunk_x;
	}

	inline int getChunkY() {
		return this->chunk_y;
	}

	inline int getChunkZ() {
		return this->chunk_z;
	}

	int getHashCode();

private:
	void flagDirtyMesh();
	
	inline Block& getBlockInternal(int x, int y, int z) {
		//std::shared_lock<std::shared_mutex> lock(this->chunkMutex);
		//this->assertBlockIndex(x, y, z);
		return data[this->toIndex(x, y, z)];
	}

	inline void setBlockInternal(int x, int y, int z, Block b) {
		data[this->toIndex(x, y, z)] = b;
	}
	
	void reinitializeChunk(int cx, int cy, int cz);

	int toIndex(int x, int y, int z);
	void assertBlockIndex(int x, int y, int z);

	int expand(int i);
};

typedef Chunk* ChunkPtr;

}

