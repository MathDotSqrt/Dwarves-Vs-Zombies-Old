#pragma once
#include "Common.h"
#include "VAO.h"
#include "VBO.h"
#include "Geometry.h"

namespace Voxel{

class ChunkMesh {
private:
	typedef Graphics::Geometry < Graphics::PositionAttrib, Graphics::NormalAttrib, Graphics::ColorAttrib> ChunkGeometry;

	const int32 cx, cy, cz;

	Graphics::VAO vao;
	Graphics::VBO vbo;
	Graphics::VBO ebo;
	
	int indexCount;

public:
	ChunkMesh(int32 cx, int32 cy, int32 cz);
	~ChunkMesh();

	void bufferGeometry(ChunkGeometry& geometry);
};

}

