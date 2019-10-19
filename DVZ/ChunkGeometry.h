#pragma once
#include "Geometry.h"
#include "attrib.h"



namespace Voxel{
class ChunkGeometry {
private:
	typedef Graphics::Attrib<POSITION_ATTRIB_LOCATION, glm::u16vec3> BlockPositionAttrib;
	typedef Graphics::Attrib<NORMAL_ATTRIB_LOCATION, glm::vec3> BlockNormalAttrib;
	typedef Graphics::Attrib<COLOR_ATTRIB_LOCATION, glm::vec3> BlockColorAttrib;

	friend class ChunkRenderData;

	typedef Graphics::Geometry<BlockPositionAttrib, BlockNormalAttrib, BlockColorAttrib> BlockGeometry;

	BlockGeometry geometry;

	static const std::tuple<BlockPositionAttrib, BlockNormalAttrib, BlockColorAttrib> ATTRIBS;
	size_t size = sizeof(BlockVertex);

public:
	typedef BlockGeometry::GeometryVertex BlockVertex;

	ChunkGeometry();
	~ChunkGeometry();

	void appendFace(BlockVertex v0, BlockVertex v1, BlockVertex v2, BlockVertex v3);
	void clear();
};
}

