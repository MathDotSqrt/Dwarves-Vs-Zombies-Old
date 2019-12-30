#pragma once
#include "Geometry.h"
#include "attrib.h"



namespace Voxel{
class ChunkGeometry {
private:
	typedef Graphics::Attrib<POSITION_ATTRIB_LOCATION, glm::u8vec4> BlockPositionAttrib;
	typedef Graphics::Attrib<NORMAL_ATTRIB_LOCATION, glm::i8vec3> BlockNormalAttrib;
	//typedef Graphics::Attrib<COLOR_ATTRIB_LOCATION, glm::u8vec3> BlockColorAttrib;
	typedef Graphics::Attrib<BLOCK_TEXCOORD_ATTRIB_LOCATION, glm::u8vec4> BlockTexcoordAttrib;
	typedef std::tuple<BlockPositionAttrib, BlockNormalAttrib, BlockTexcoordAttrib> AttribTuple;

	typedef Graphics::Geometry<BlockPositionAttrib, BlockNormalAttrib, BlockTexcoordAttrib> BlockGeometry;
	//BlockGeometry geometry;

public:
	typedef BlockGeometry::GeometryVertex BlockVertex;
	static const AttribTuple ATTRIBS;


	ChunkGeometry();
	~ChunkGeometry();

	void appendFace(BlockVertex v0, BlockVertex v1, BlockVertex v2, BlockVertex v3);
	void clear();

	const std::vector<BlockVertex>& getBlockGeometry();

private:
	std::vector<BlockVertex> geometry;
};
}

