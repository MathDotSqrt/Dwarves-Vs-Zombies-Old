#pragma once
#include "Geometry.h"
#include "attrib.h"

#define BLOCK_ATTRIBS	Graphics::Attrib<POSITION_ATTRIB_LOCATION, glm::u8vec3>, \
						Graphics::Attrib<NORMAL_ATTRIB_LOCATION, glm::vec3>,   \
						Graphics::Attrib<COLOR_ATTRIB_LOCATION, glm::vec3>       \

namespace Voxel{
class ChunkGeometry {
private:
	friend class ChunkRenderData;

	typedef Graphics::Geometry<BLOCK_ATTRIBS> BlockGeometry;

	BlockGeometry geometry;

	static const std::tuple<BLOCK_ATTRIBS> ATTRIBS;

public:
	typedef BlockGeometry::GeometryVertex BlockVertex;

	ChunkGeometry();
	~ChunkGeometry();

	void appendFace(BlockVertex v0, BlockVertex v1, BlockVertex v2, BlockVertex v3);
	void clear();
};
}

