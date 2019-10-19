#include "ChunkGeometry.h"

using namespace Voxel;

ChunkGeometry::ChunkGeometry() {
}


ChunkGeometry::~ChunkGeometry() {
}

void ChunkGeometry::appendFace(ChunkGeometry::BlockVertex v0, ChunkGeometry::BlockVertex v1, ChunkGeometry::BlockVertex v2, ChunkGeometry::BlockVertex v3) {
	geometry.pushVertex(v0);
	geometry.pushVertex(v1);
	geometry.pushVertex(v2);
	geometry.pushVertex(v3);

	////todo figure out if this is correct
	int lastIndex = (int)geometry.getVertexCount();

	geometry.pushTriangle(lastIndex + 0, lastIndex + 1, lastIndex + 2);
	geometry.pushTriangle(lastIndex + 0, lastIndex + 2, lastIndex + 3);
}

void ChunkGeometry::clear() {
	this->geometry.clear();
}

const std::tuple<BLOCK_ATTRIBS> ChunkGeometry::ATTRIBS = ChunkGeometry::BlockGeometry().attribs;