#pragma once
#include "Block.h"
#include <queue>
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

//when queuing chunks to get meshed dont queue the ones who need a light updating
enum class LightState{
	NONE,
	LOADED,
	LOADED_AND_EMPTY,
	LOCKED,
	NUM_LIGHT_STATES
};

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

class ChunkManager;

struct Light {
	uint8 value;

	Light() {
		value = 0;
	}

	Light(uint8 sunlight, uint8 blocklight) {
		value = ((sunlight & 0xf) << 4) | (blocklight & 0xf);
	}

	bool operator==(const Light &other) const{
		return value == other.value;
	}

	bool operator!=(const Light &other)  const {
		return value != other.value;
	}

	operator uint8() const {
		return value;
	}

	uint8 getBlockLight() {
		return value & 0xf;
	}

	void setBlockLight(uint8 torch) {
		value = (value & 0xf0) | (torch & 0xf);
	}

	uint8 getSunLight() {
		return (value >> 4) & 0xf;
	}

	void setSunLight(uint8 sun) {
		value = ((sun & 0xf) << 4) | (value & 0xf);
	}
};

//DO NOT CALL THIS ON STACK
class Chunk {

private:
	friend class ChunkMesher;
	friend class ChunkLightEngine;
	friend class ChunkManager;

	struct LightNode {
		Light light;
		int32 x, y, z;
	};

	std::deque<LightNode> lightQueue;

	Block blockData[CHUNK_VOLUME];
	Light lightData[CHUNK_VOLUME];

	BlockState blockState;
	MeshState meshState;

	std::shared_mutex chunkMutex;

	int chunk_x, chunk_y, chunk_z;


	ChunkManager *manager;

public:
	
	//todo make chunk contain shared pointer of render data from a recycler. Maybe
	Chunk(int x, int y, int z, ChunkManager *manager);
	~Chunk();

	void generateTerrain();

	void flagMeshValid();
	void flagMeshRemoved();
	void flagDirtyMesh();

	bool isEmpty();
	
	BlockState getBlockState();
	MeshState getMeshState();

	BlockState tryGetBlockState();
	MeshState tryGetMeshState();

	Block getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block block);			//must do it on main thread

	Light getLight(int x, int y, int z);
	void setLight(int x, int y, int z, Light light);		//must do it in main thread
	void setSunLight(int x, int y, int z, uint8 sun);		//must do it in main thread
	void setBlockLight(int x, int y, int z, uint8 block);	//must do it in main thread

	inline int getChunkX() const {
		return this->chunk_x;
	}

	inline int getChunkY() const {
		return this->chunk_y;
	}

	inline int getChunkZ() const {
		return this->chunk_z;
	}

	static constexpr int calcHashCode(int cx, int cy, int cz) {
		return expand(cx) + (expand(cy) << 1) + (expand(cz) << 2);
	}

	static constexpr bool isIndexInBounds(int x, int y, int z);
	int getHashCode();

private:
	void flagDirtyMeshInternal();

	Light getBrightestNeighbor(int x, int y, int z) const;

	inline Block getBlockInternal(int x, int y, int z) const {
		return blockData[toIndex(x, y, z)];
	}

	inline void setBlockInternal(int x, int y, int z, Block b) {
		blockData[toIndex(x, y, z)] = b;
	}
	
	inline Light getLightInternal(int x, int y, int z) const {
		return lightData[toIndex(x, y, z)];
	}

	inline void setLightInternal(int x, int y, int z, Light light) {
		lightData[toIndex(x, y, z)] = light;
	}

	inline void queueLightInternal(int x, int y, int z, Light light) {
		LightNode node = { light, x, y, z };
		lightQueue.push_back(node);
	}

	void reinitializeChunk(int cx, int cy, int cz);					//todo find a code patter to get rid of this

	inline static constexpr int toIndex(int x, int y, int z) {
		return x + CHUNK_WIDTH_X * (y + CHUNK_WIDTH_Y * z);
	}

	void assertBlockIndex(int x, int y, int z) const;

	static constexpr int expand(int x) {
		x &= 0x3FF;
		x = (x | (x << 16)) & 4278190335;
		x = (x | (x << 8)) & 251719695;
		x = (x | (x << 4)) & 3272356035;
		x = (x | (x << 2)) & 1227133513;
		return x;
	}
};

typedef Chunk* ChunkPtr;

}

