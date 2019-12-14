#pragma once
#include "common.h"
#include "TEX.h"

namespace Graphics {
	class FBO {
	private:
		uint32 fboID = 0;
		TEX color;
		TEX depth;

	public:
		FBO(int width, int height);
		FBO(const FBO &) = delete;
		FBO(FBO&& other);
		~FBO();

		FBO& operator=(const FBO &) = delete;
		FBO& operator&=(FBO&&);

		void dispose();

		void bind();
		void unbind();

		void bindTexture(int unit);

		int getWidth();
		int getHeight();

	};
}


