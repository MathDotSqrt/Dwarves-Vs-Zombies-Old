#include "ChunkRenderData.h"

using namespace Voxel;

ChunkRenderData::ChunkRenderData() : 
	vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER) {
	if (ChunkRenderData::CHUNK_EBO == nullptr) {
		CHUNK_EBO = std::unique_ptr<Graphics::VBO>(new Graphics::VBO(GL_ELEMENT_ARRAY_BUFFER)); 
		expandMasterEBO(60000);
	}

	this->vao.bind();
	this->vao.bufferInterleavedData(this->vbo, ChunkGeometry::ATTRIBS);
	//this->ebo.bind();
	ChunkRenderData::CHUNK_EBO->bind();
	this->vao.unbind();
	ChunkRenderData::CHUNK_EBO->unbind();
	//this->ebo.unbind();

	this->indexCount = 0;
}


ChunkRenderData::~ChunkRenderData() {

}

void ChunkRenderData::bufferGeometry(ChunkGeometry *geometry) {
	size_t vertexCount = geometry->geometry.getVertexCount();
	size_t numQuads = vertexCount / 4;
	size_t currentNumQuads = ChunkRenderData::indices.size();
	if (numQuads > currentNumQuads) {
		size_t newSize = (currentNumQuads * 3) / 2;
		expandMasterEBO(newSize);
	}
	
	this->vbo.bind();
	this->vbo.bufferData(geometry->geometry.getVerticies(), GL_STATIC_DRAW);
	//this->ebo.bind();
	//this->ebo.bufferData(geometry->geometry.getIndices(), GL_STATIC_DRAW);
	//this->ebo.unbind();

	this->indexCount = geometry->geometry.getIndexCount();
}

void ChunkRenderData::expandMasterEBO(size_t newQuadSize) {
	LOG_VOXEL("EXPAND %d", newQuadSize);
	ChunkRenderData::indices.reserve(newQuadSize);
	for (int i = ChunkRenderData::indices.size(); i < newQuadSize; i++) {
		//there are four quads for every index. quad indices is made up of 6 indices
		//that will be the indices for a quad of four vertexes
		ChunkRenderData::indices.push_back(QuadIndicies(i*4));
	}

	ChunkRenderData::CHUNK_EBO->bind();
	ChunkRenderData::CHUNK_EBO->bufferData(ChunkRenderData::indices, GL_STATIC_DRAW);
	ChunkRenderData::CHUNK_EBO->unbind();

}

std::unique_ptr<Graphics::VBO> ChunkRenderData::CHUNK_EBO = nullptr;
std::vector<ChunkRenderData::QuadIndicies> ChunkRenderData::indices;
