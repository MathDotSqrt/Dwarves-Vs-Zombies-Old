#pragma once
#include "Geometry.h"
#include <string.h>


namespace Graphics {

	struct ModelVertex {
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};

	class ModelGeometry : public Geometry<ModelVertex> {
	private:

	
	public:
		ModelGeometry(std::string filename);
		~ModelGeometry();
	};
}

