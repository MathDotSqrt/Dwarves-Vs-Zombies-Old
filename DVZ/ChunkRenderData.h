#pragma once
#include "Common.h"
#include "VAO.h"
#include "VBO.h"
#include "Geometry.h"

namespace Voxel{

class ChunkRenderData {
private:
	typedef Graphics::Geometry < Graphics::PositionAttrib, Graphics::NormalAttrib, Graphics::ColorAttrib> ChunkGeometry;

	const int32 cx, cy, cz;

	Graphics::VAO vao;
	Graphics::VBO vbo;
	Graphics::VBO ebo;
	
	size_t indexCount;

public:
	ChunkRenderData(int32 cx, int32 cy, int32 cz);
	~ChunkRenderData();

	void bufferGeometry(ChunkGeometry& geometry);
};

}

