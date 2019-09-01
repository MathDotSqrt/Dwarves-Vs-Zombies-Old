#pragma once
#include <vector>
#include "VAO.h"
#include "VBO.h"

namespace Graphics {


template<typename VERTEX>
class Geometry {
protected:
	std::vector<VERTEX> verticies;
	std::vector<unsigned int> indices;

	VAO vao;
	VBO vbo;
	VBO ebo;

public:
	Geometry() {

	}
	~Geometry() {
	
	}

	//todo add appendQuad method

	template<typename VERTEX>
	inline void pushVertex(VERTEX &vertex) {
		verticies.push_back(vertex);
	}

	inline void pushTriangle(unsigned int v0, unsigned int v1, unsigned int v2) {
		indices.push_back(v0);
		indices.push_back(v1);
		indices.push_back(v2);
	}

	inline void clear() {
		verticies.clear();
		indices.clear();
	}

	inline std::vector<VERTEX>& getVerticies() {
		return this->verticies;
	}

	inline std::vector<unsigned int>& getIndices() {
		return this->indices;
	}

	inline size_t getVertexCount() {
		return verticies.size();
	}

	inline size_t getIndexCount() {
		return indices.size();
	}
};
}
