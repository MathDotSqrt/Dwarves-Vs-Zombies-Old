#pragma once
#include "common.h"

namespace Graphics {
	class FBO {
	protected:
		uint32 fboID = 0;
		int width = 0;
		int height = 0;

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

		int getWidth();
		int getHeight();

	};
}


