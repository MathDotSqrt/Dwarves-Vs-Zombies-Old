#pragma once
#include "Block.h"
#include "VAO.h"
#include <vector>

namespace Voxel{

class Chunk {

private:
	const int chunk_x, chunk_y, chunk_z;
	
	Block *data;
	bool isEmpty;
	
	std::vector<BlockVertex> verticies;
	std::vector<GLuint> indices;
	Graphics::VAO vao;
	int vertexCount = 0;



public:
	static constexpr int CHUNK_SIZE = 16;
	static constexpr float CHUNK_RENDER_SIZE = Chunk::CHUNK_SIZE * BLOCK_RENDER_SIZE;

	Chunk(int x, int y, int z);
	~Chunk();

	void generateTerrain();
	void generateMesh();

	inline Block& getBlock(int x, int y, int z);
};

}

