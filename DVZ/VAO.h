#pragma once
#include <vector>
#include <algorithm>
#include "macrologger.h"

#include <GL/glew.h>
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
	}

	void bind();
	void unbind();


	template<typename VERTEX, typename ATTRIB>
	void bufferData(std::vector<VERTEX> verticies, GLenum bufferHint) {
		this->bind();

	}
};

}

