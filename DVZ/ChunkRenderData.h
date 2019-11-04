#pragma once
#include "Common.h"
#include "VAO.h"
#include "VBO.h"
#include "Geometry.h"
#include "ChunkGeometry.h"

namespace Voxel{

class ChunkRenderData {
private:
	static size_t CHUNK_EBO_SIZE;
	static size_t CHUNK_EBO_VERTEX_COUNT;
public:
	static std::unique_ptr<Graphics::VBO> CHUNK_EBO;


	int32 cx, cy, cz;

	Graphics::VAO vao;
	Graphics::VBO vbo;
	Graphics::VBO ebo;

	size_t indexCount;

	ChunkRenderData();
	~ChunkRenderData();

	void bufferGeometry(ChunkGeometry *geometry);



	inline int getChunkX() {
		return cx;
	}
	
	inline int getChunkY() {
		return cy;
	}

	inline int getChunkZ() {
		return cz;
	}
private:

	void expandMasterEBO(size_t newSize);
};

}

