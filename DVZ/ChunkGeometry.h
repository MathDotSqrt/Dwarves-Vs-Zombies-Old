#pragma once
#include "Geometry.h"
#include "attrib.h"



namespace Voxel{
class ChunkGeometry {
private:
	typedef Graphics::Attrib<POSITION_ATTRIB_LOCATION, glm::u8vec3> BlockPositionAttrib;
	typedef Graphics::Attrib<NORMAL_ATTRIB_LOCATION, glm::i8vec3> BlockNormalAttrib;
	typedef Graphics::Attrib<COLOR_ATTRIB_LOCATION, glm::u8vec3> BlockColorAttrib;


	typedef Graphics::Geometry<BlockPositionAttrib, BlockNormalAttrib, BlockColorAttrib> BlockGeometry;
	BlockGeometry geometry;

public:
	typedef BlockGeometry::GeometryVertex BlockVertex;
	static const std::tuple<BlockPositionAttrib, BlockNormalAttrib, BlockColorAttrib> ATTRIBS;


	ChunkGeometry();
	~ChunkGeometry();

	void appendFace(BlockVertex v0, BlockVertex v1, BlockVertex v2, BlockVertex v3);
	void clear();

	const BlockGeometry& getBlockGeometry();
};
}

