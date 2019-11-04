#include "ChunkGeometry.h"

using namespace Voxel;

ChunkGeometry::ChunkGeometry() : geometry(
	ChunkGeometry::BlockPositionAttrib(
		ChunkGeometry::BlockPositionAttrib::DataType::UNSIGNED_BYTE,
		ChunkGeometry::BlockPositionAttrib::Components::FOUR
	),
	ChunkGeometry::BlockNormalAttrib(
		ChunkGeometry::BlockNormalAttrib::DataType::BYTE, 
		ChunkGeometry::BlockNormalAttrib::Components::FOUR
	),
	ChunkGeometry::BlockColorAttrib(
		ChunkGeometry::BlockColorAttrib::DataType::UNSIGNED_BYTE,
		ChunkGeometry::BlockColorAttrib::Components::FOUR,
		ChunkGeometry::BlockColorAttrib::AttribOption::Normalize
	)
){}


ChunkGeometry::~ChunkGeometry() {
}

void ChunkGeometry::appendFace(ChunkGeometry::BlockVertex v0, ChunkGeometry::BlockVertex v1, ChunkGeometry::BlockVertex v2, ChunkGeometry::BlockVertex v3) {
	int lastIndex = (int)geometry.getVertexCount();

	
	geometry.pushVertex(v0);
	geometry.pushVertex(v1);
	geometry.pushVertex(v2);
	geometry.pushVertex(v3);

	////todo figure out if this is correct

	geometry.pushTriangle(lastIndex + 0, lastIndex + 1, lastIndex + 2);
	geometry.pushTriangle(lastIndex + 0, lastIndex + 2, lastIndex + 3);
}

void ChunkGeometry::clear() {
	this->geometry.clear();
}

const ChunkGeometry::BlockGeometry& ChunkGeometry::getBlockGeometry() {
	return this->geometry;
}

const std::tuple<ChunkGeometry::BlockPositionAttrib, ChunkGeometry::BlockNormalAttrib, ChunkGeometry::BlockColorAttrib> ChunkGeometry::ATTRIBS = ChunkGeometry().geometry.attribs;