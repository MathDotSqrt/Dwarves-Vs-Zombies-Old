#pragma once

#include "VAO.h"
#include "VBO.h"
#include "Geometry.h"

namespace Graphics{

class Mesh {
private:
	VAO vao;
	VBO vbo;
	VBO ebo;

public:
	Mesh();
	~Mesh();


};

}