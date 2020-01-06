#include "ChunkRenderData.h"

using namespace Voxel;

ChunkRenderData::ChunkRenderData() : 
	vbo(GL_ARRAY_BUFFER){
	if (ChunkRenderData::CHUNK_EBO == nullptr) {
		CHUNK_EBO = std::unique_ptr<Graphics::VBO>(new Graphics::VBO(GL_ELEMENT_ARRAY_BUFFER)); 
		expandMasterEBO(60000);
	}

	this->vao.bind();
	this->vbo.bind();
	this->vao.bufferInterleavedData(ChunkGeometry::ATTRIBS);
	this->vbo.unbind();
	ChunkRenderData::CHUNK_EBO->bind();
	this->vao.unbind();
	ChunkRenderData::CHUNK_EBO->unbind();

	this->indexCount = 0;
}


ChunkRenderData::~ChunkRenderData() {

}

void ChunkRenderData::bufferGeometry(ChunkGeometry *geometry) {
	size_t vertexCount = geometry->getBlockGeometry().size();
	size_t numQuads = vertexCount / 4;
	size_t currentNumQuads = ChunkRenderData::indices.size();
	if (numQuads > currentNumQuads) {
		size_t newSize = (currentNumQuads * 3) / 2;
		expandMasterEBO(newSize);
	}
	
	this->vbo.bind();
	this->vbo.bufferData(geometry->getBlockGeometry(), GL_STATIC_DRAW);

	//six indicies for every quad
	this->indexCount = numQuads * 6;
}

void ChunkRenderData::expandMasterEBO(size_t newQuadSize) {
	LOG_VOXEL("EXPAND %zd", newQuadSize);
	ChunkRenderData::indices.reserve(newQuadSize);
	for (size_t i = ChunkRenderData::indices.size(); i < newQuadSize; i++) {
		//there are four quads for every index. quad indices is made up of 6 indices
		//that will be the indices for a quad of four vertexes
		ChunkRenderData::indices.push_back(QuadIndicies((ChunkRenderData::IndexType)i*4));
	}

	ChunkRenderData::CHUNK_EBO->bind();
	ChunkRenderData::CHUNK_EBO->bufferData(ChunkRenderData::indices, GL_STATIC_DRAW);
	ChunkRenderData::CHUNK_EBO->unbind();

}


std::unique_ptr<Graphics::VBO> ChunkRenderData::CHUNK_EBO = nullptr;
std::vector<ChunkRenderData::QuadIndicies> ChunkRenderData::indices;
