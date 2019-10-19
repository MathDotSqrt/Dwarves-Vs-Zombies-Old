#pragma once
#include "Common.h"
#include "VAO.h"
#include "VBO.h"
#include "Geometry.h"
#include "ChunkGeometry.h"

namespace Voxel{

class ChunkRenderData {
private:

public:
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
};

}

