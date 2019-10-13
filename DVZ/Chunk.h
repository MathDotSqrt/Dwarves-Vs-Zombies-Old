#pragma once
#include "Block.h"
#include "VAO.h"
#include "VBO.h"
#include <vector>
#include "Geometry.h"
#include "Attrib.h"
#include <shared_mutex>

namespace Voxel{

static constexpr int CHUNK_SHIFT_X = 4;
static constexpr int CHUNK_SHIFT_Y = 6;
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

//DO NOT CALL THIS ON STACK
class Chunk {
public:
	typedef Graphics::Geometry<Graphics::PositionAttrib, Graphics::NormalAttrib, Graphics::ColorAttrib> BlockGeometry;
	typedef BlockGeometry::GeometryVertex BlockVertex;

	typedef enum _ChunkState{
		EMPTY,					//allocated but all of the values are nonsense
		LAZY_LOADED,			//allocated with generated blocks, but has no render data associated. Useful for meshing algo
		NEED_MESH,				//allocated but needs mesh
		DIRTY_MESH,				//has a mesh but the chunk state is different than the mesh
		VALID					//has a mesh and chunk is consistant with mesh
	} ChunkState;

private:
	friend class ChunkMesher;

	const int chunk_x, chunk_y, chunk_z;
	
	Block data[CHUNK_VOLUME];
	ChunkState currentState;
	BlockGeometry geometry;

	std::shared_mutex chunkMutex;

public:
	
	Chunk(int x, int y, int z);
	~Chunk();

	void generateTerrain();

	Block getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block block);

	//needs mesh
	inline bool needsNewMesh() {
		return this->currentState == Chunk::NEED_MESH;
	}
	inline bool needsMeshUpdate() {
		return this->currentState == Chunk::DIRTY_MESH;
	}
	
	inline bool flagLoadLazy() {
		if (this->currentState == Chunk::EMPTY) {
			this->currentState = Chunk::LAZY_LOADED;
			return true;
		}

		return false;
	}

	inline bool flagMeshCreation() {
		if (this->currentState == Chunk::ChunkState::LAZY_LOADED) {
			this->currentState = Chunk::NEED_MESH;
			return true;
		}

		return false;
	}

	inline bool flagDirty() {
		if (this->currentState == Chunk::ChunkState::VALID) {
			this->currentState = Chunk::ChunkState::DIRTY_MESH;
			return true;
		}
		return false;
	}

	inline bool flagValid() {
		this->currentState = Chunk::ChunkState::VALID;
		return true;
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

	inline ChunkState getChunkState() {
		return this->currentState;
	}

	inline BlockGeometry& getChunkGeometry() {
		return this->geometry;
	}

private:
	inline Block& getBlockInternal(int x, int y, int z) {
		//std::shared_lock<std::shared_mutex> lock(this->chunkMutex);
		//this->assertBlockIndex(x, y, z);
		return data[this->toIndex(x, y, z)];
	}
	int toIndex(int x, int y, int z);
	void assertBlockIndex(int x, int y, int z);
};

}

