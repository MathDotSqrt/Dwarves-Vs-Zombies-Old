#pragma once
#include "IRenderer.h"

namespace Graphics {
class OpenGLRenderer : public IRenderer
{
public:
	OpenGLRenderer();
	~OpenGLRenderer();

	void init(Scene *scene) override;
	void resize(int newWidth, int newHeight) override;
	void prerender() override;
	void render() override;
};
}

