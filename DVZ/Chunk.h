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
	bool isEmpty;
	
	std::vector<BlockVertex> verticies;
	std::vector<GLuint> indices;
	Graphics::VBO vbo;
	Graphics::VBO ebo;

	inline void createCube(float x, float y, float z, BlockFaceCullTags render, BlockType type);
	inline void createFace(BlockVertex v0, BlockVertex v1, BlockVertex v2, BlockVertex v3);

public:
	static constexpr int CHUNK_SIZE = 64;
	static constexpr float CHUNK_RENDER_SIZE = Chunk::CHUNK_SIZE * BLOCK_RENDER_SIZE;

	Graphics::VAO vao;
	int indexCount = 0;


	Chunk(int x, int y, int z);
	~Chunk();

	void generateTerrain();
	void generateMesh();


	inline Block& getBlock(int x, int y, int z);
};

}

