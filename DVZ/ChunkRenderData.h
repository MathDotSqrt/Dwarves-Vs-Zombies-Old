#pragma once
#include "Common.h"
#include "VAO.h"
#include "VBO.h"
#include "Geometry.h"
#include "ChunkGeometry.h"

namespace Voxel{

class ChunkRenderData {
private:
	typedef int IndexType;
	
	struct QuadIndicies {
		IndexType i0;
		IndexType i1;
		IndexType i2;
		IndexType i3;
		IndexType i4;
		IndexType i5;

		QuadIndicies(int startIndex) {
			i0 = startIndex + 0;
			i1 = startIndex + 1;
			i2 = startIndex + 2;
			i3 = startIndex + 0;
			i4 = startIndex + 2;
			i5 = startIndex + 3;
		}
	};

	static std::vector<QuadIndicies> indices;
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

	void expandMasterEBO(size_t newQuadSize);
};

}

