#include "ChunkGeometry.h"

using namespace Voxel;

ChunkGeometry::ChunkGeometry() : geometry(
	
){}


ChunkGeometry::~ChunkGeometry() {
}

void ChunkGeometry::appendFace(ChunkGeometry::BlockVertex v0, ChunkGeometry::BlockVertex v1, ChunkGeometry::BlockVertex v2, ChunkGeometry::BlockVertex v3) {
	geometry.push_back(v0);
	geometry.push_back(v1);
	geometry.push_back(v2);
	geometry.push_back(v3);
}

void ChunkGeometry::clear() {
	this->geometry.clear();
}

const std::vector<ChunkGeometry::BlockVertex>& ChunkGeometry::getBlockGeometry() {
	return this->geometry;
}

const ChunkGeometry::AttribTuple ChunkGeometry::ATTRIBS = std::make_tuple(
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
);