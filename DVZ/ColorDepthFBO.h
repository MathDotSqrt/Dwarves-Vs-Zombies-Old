#pragma once
#include "FBO.h"
#include "TEX.h"

namespace Graphics {
	class ColorDepthFBO : public FBO {
	private:
		TEX color;
		TEX depth;
	public:
		ColorDepthFBO(int width, int height);
		ColorDepthFBO(const ColorDepthFBO &) = delete;
		ColorDepthFBO(ColorDepthFBO &&other);
		~ColorDepthFBO();

		ColorDepthFBO& operator&=(ColorDepthFBO &&other);

		TEX& getColorAttachment();
		TEX& getDepthAttachment();


	};
}


