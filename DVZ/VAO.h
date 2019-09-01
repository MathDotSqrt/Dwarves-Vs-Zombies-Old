#pragma once
#include <vector>
#include <algorithm>
#include "macrologger.h"
#include <GL/glew.h>
#include "Attrib.h"
#include "VBO.h"

namespace Graphics {

class VAO {
private:
	GLuint vaoID;
	void dispose();

public:
	VAO();
	~VAO();
	VAO(VAO &&other);

	VAO(const VAO &) = delete;
	VAO& operator=(const VAO &) = delete;

	VAO& operator=(VAO &&other) {
		if (this != &other) {
			this->dispose();
			//this->vaoID is zero
			std::swap(this->vaoID, other.vaoID);
		}
		
		//todo figure this out
		return *this;
	}

	void bind();
	void unbind();


	template<typename ...T>
	void bufferInterleavedData(VBO &vbo, const T&... attribs) {
		vbo.bind();
		
		size_t stride = this->getAttribsStride(attribs...);
		//this->setInterleavedAttribPointers(stride, 0, attribs...);

		vbo.unbind();
	}
private:

	template<unsigned int L, typename T, typename ...U>
	void setInterleavedAttribPointers(size_t stride, size_t offset, const Attrib<L, T>& attrib, const U&... attribs) {
		addVertexAttrib(attrib, stride, offset);

		this->setInterleavedAttribPointers(stride, offset + attrib.getSizeOfAttrib(), attribs...);
	}
	
	template<unsigned int L, typename T>
	void setInterleavedAttribPointers(size_t stride, size_t offset, const Attrib<L, T>& attrib) {
		addVertexAttrib(attrib, stride, offset);
	}

	void setInterleavedAttribPointers(size_t stride, size_t offset) {}


	template<unsigned int L, typename T>
	void addVertexAttrib(const Attrib<L, T>& attrib, size_t stride, size_t offset) {
		//todo figure out how to handle matrices
		glVertexAttribPointer(L, (int)attrib.getNumComponents(), attrib.getScalarType(), attrib.getAttribOption(), stride, (void*)offset);
	}

	template<unsigned int L, typename T, typename ...U>
	size_t getAttribsStride(const Attrib<L, T>& attrib, const U&... attribs) {
		return attrib.getSizeOfAttrib() + this->getAttribsStride(attribs...);
	}

	template<unsigned int L, typename T>
	size_t getAttribsStride(const Attrib<L, T>& attrib) {
		return attrib.getSizeOfAttrib();
	}

	size_t getAttribsStride() {
		return 0;
	}

};

}

