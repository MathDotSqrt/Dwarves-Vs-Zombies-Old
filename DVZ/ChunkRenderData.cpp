#include "ChunkRenderData.h"

using namespace Voxel;

ChunkRenderData::ChunkRenderData() : 
	vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER) {
	this->vao.bind();
	this->vao.bufferInterleavedData(this->vbo, ChunkGeometry::ATTRIBS);
	this->ebo.bind();
	this->vao.unbind();
	this->ebo.unbind();

	this->indexCount = 0;
}


ChunkRenderData::~ChunkRenderData() {

}

void ChunkRenderData::bufferGeometry(ChunkGeometry *geometry) {
	this->vbo.bind();
	this->vbo.bufferData(geometry->geometry.getVerticies(), GL_STATIC_DRAW);
	this->ebo.bind();
	this->ebo.bufferData(geometry->geometry.getIndices(), GL_STATIC_DRAW);
	this->ebo.unbind();

	this->indexCount = geometry->geometry.getIndexCount();
}
