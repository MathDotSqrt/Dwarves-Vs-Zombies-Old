#include "ChunkMesh.h"

using namespace Voxel;

ChunkMesh::ChunkMesh(int32 cx, int32 cy, int32 cz) : 
	vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER), 
	cx(cx), cy(cy), cz(cz){
	this->vao.bind();
	this->vao.bufferInterleavedData(this->vbo, Graphics::PositionAttrib(), Graphics::NormalAttrib(), Graphics::ColorAttrib());
	this->ebo.bind();
	this->vao.unbind();
	this->ebo.unbind();

	this->indexCount = 0;
}


ChunkMesh::~ChunkMesh() {

}

void ChunkMesh::bufferGeometry(ChunkMesh::ChunkGeometry& geometry) {
	this->vbo.bind();
	this->vbo.bufferData(geometry.getVerticies(), GL_STATIC_DRAW);
	this->ebo.bind();
	this->ebo.bufferData(geometry.getIndices(), GL_STATIC_DRAW);
	this->ebo.unbind();

	this->indexCount = geometry.getIndexCount();
}
