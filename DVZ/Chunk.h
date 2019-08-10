#pragma once
#include "Block.h"
#include "VAO.h"
#include "VBO.h"
#include <vector>

namespace Voxel{

class Chunk {
private:
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
	
	std::vector<BlockVertex> verticies;
	std::vector<GLuint> indices;
	Graphics::VBO vbo;
	Graphics::VBO ebo;

public:
	static constexpr int CHUNK_SHIFT_X = 6;
	static constexpr int CHUNK_SHIFT_Y = 6;
	static constexpr int CHUNK_SHIFT_Z = 6;

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

	Graphics::VAO vao;
	int indexCount = 0;

	Chunk(int x, int y, int z);
	Chunk(int x, int y, int z, Block *data);
	~Chunk();

	void generateTerrain();
	void generateMesh();


	inline Block& getBlock(int x, int y, int z);
	inline void setBlock(int x, int y, int z, Block &block);
	inline void setBlockData(Block *newData);

private:
	inline void createCube(int x, int y, int z, BlockFaceCullTags render, BlockType type);
	inline void createFace(BlockVertex v0, BlockVertex v1, BlockVertex v2, BlockVertex v3);

	inline int toIndex(int x, int y, int z);
	inline void assertBlockIndex(int x, int y, int z);
};

}

