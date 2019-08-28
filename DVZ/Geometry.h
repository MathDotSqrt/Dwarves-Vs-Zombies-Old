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

	inline int getVertexCount() {
		return verticies.size();
	}

	inline int getIndexCount() {
		return indices.size();
	}
};
}
