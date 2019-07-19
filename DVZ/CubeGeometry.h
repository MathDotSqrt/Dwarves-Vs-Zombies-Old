#pragma once
#include "Geometry.h"
namespace Graphics {
	class CubeGeometry :
		public Geometry
	{
	private:
		void init();

	public:
		CubeGeometry();
		~CubeGeometry();


	};
}
