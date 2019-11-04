#include "ChunkRenderData.h"

using namespace Voxel;

ChunkRenderData::ChunkRenderData() : 
	vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER) {
	if (ChunkRenderData::CHUNK_EBO == nullptr) {
		CHUNK_EBO = std::unique_ptr<Graphics::VBO>(new Graphics::VBO(GL_ELEMENT_ARRAY_BUFFER)); 
		expandMasterEBO(100000);
	}

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
	size_t vertexCount = geometry->geometry.getVertexCount();
	size_t indexCount = 6 * vertexCount / 4;	//6 indices for every quad

	if (indexCount > ChunkRenderData::CHUNK_EBO_SIZE) {
		size_t newSize = (ChunkRenderData::CHUNK_EBO_SIZE * 3) / 2;
		expandMasterEBO(newSize);
	}
	
	this->vbo.bind();
	this->vbo.bufferData(geometry->geometry.getVerticies(), GL_STATIC_DRAW);
	this->ebo.bind();
	this->ebo.bufferData(geometry->geometry.getIndices(), GL_STATIC_DRAW);
	this->ebo.unbind();

	this->indexCount = geometry->geometry.getIndexCount();
}

void ChunkRenderData::expandMasterEBO(size_t newSize) {
	int startIndex = newSize;
}

std::unique_ptr<Graphics::VBO> ChunkRenderData::CHUNK_EBO = nullptr;
size_t ChunkRenderData::CHUNK_EBO_SIZE = 0;
size_t ChunkRenderData::CHUNK_EBO_VERTEX_COUNT = 0;
