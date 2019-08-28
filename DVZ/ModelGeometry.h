#pragma once
#include "Geometry.h"
#include "OBJLoader.h"
#include <string.h>


namespace Graphics {
	class ModelGeometry : public Geometry<Loader::Vertex> {
	private:

	
	public:
		ModelGeometry(std::string filename);
		~ModelGeometry();
	};
}

