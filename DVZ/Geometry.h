#pragma once
#include <vector>
#include <tuple>
#include "glm.hpp"

namespace Graphics {

template<typename ...T>
class Geometry {
public:
	template<typename ...V>
	struct Vertex {};
	//drop this
	template<typename V, typename ...REST>
	struct Vertex<V, REST...>{
		Vertex(
			typename const V::Type& first,
			typename const REST::Type& ...rest)
			: first(first), rest(rest...) {}
		
		typename V::Type first;
		Vertex<REST...> rest;
	};

	template<typename V>
	struct Vertex<V> {
		Vertex( typename const V::Type& last) : last(last){}

		typename V::Type last;
	};
	
	typedef Vertex<T...> GeometryVertex;
protected:
	std::vector<GeometryVertex> verticies;
	std::vector<unsigned int> indices;

public:
	std::tuple<T...> attribs;

	Geometry(){

	}

	Geometry(T... attribs) : attribs(attribs...){
		
	}

	~Geometry() {
	
	}

	//todo add appendQuad method
	inline void pushVertex(typename const T::Type& ... vertexData) {
		verticies.push_back(GeometryVertex(vertexData...));
	}

	inline void pushVertex(const GeometryVertex &vertex) {
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

	inline std::vector<GeometryVertex>& getVerticies() {
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