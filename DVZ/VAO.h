#pragma once
#include <vector>
#include <tuple>
#include <algorithm>
#include "macrologger.h"
#include <GL/glew.h>
#include "Attrib.h"
#include "VBO.h"

namespace Graphics {

class VAO {
private:
	GLuint vaoID = 0;
	void dispose();

public:
	VAO();
	~VAO();
	VAO(VAO &&other);

	VAO(const VAO &) = delete;
	VAO& operator=(const VAO &) = delete;

	VAO& operator=(VAO &&other);

	void bind();
	void unbind();


	template<typename ...T>
	void bufferInterleavedData(const T&... attribs) {
		size_t stride = this->getAttribsStride(attribs...);
		this->setInterleavedAttribPointers(stride, 0, attribs...);
	}

	template<typename ...T>
	void bufferInterleavedData(const std::tuple<T...>& attribs) {
		auto lambda = [this](auto &&...args) {
			size_t stride = this->getAttribsStride(args...);
			this->setInterleavedAttribPointers(stride, 0, args...);
		};
		std::apply(lambda, attribs);
	}

	GLuint getID() const;
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
		glVertexAttribPointer(L, attrib.getNumComponents(), attrib.getDataType(), attrib.getAttribOption(), (GLsizei)stride, (void*)offset);
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

