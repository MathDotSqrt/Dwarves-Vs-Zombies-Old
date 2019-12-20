#pragma once
#include "IRenderer.h"
#include "RenderStateKey.h"
#include "TEX.h"
#include "VAO.h"
#include "VBO.h"
#include "FBO.h"

namespace Graphics {

class OpenGLRenderer : public IRenderer {
private:


	std::vector<RenderStateKey> sortedRenderStateKeys;
	glm::mat4 perspectiveProjection;


	double start;
	double duration;

	int window_width = 0;
	int window_height = 0;
	
	ViewPort currentPort;
	FBO shadow;
	FBO final;
	VAO quad;
	VBO vbo;

	void swapViewPorts(RenderStateKey key);
	void renderPostProcess();
	float getShaderTime();

public:
	OpenGLRenderer();
	~OpenGLRenderer();

	void init(Scene *scene) override;
	void resize(int newWidth, int newHeight) override;
	void prerender() override;
	void render(Voxel::ChunkRenderDataManager *manager) override;
	void postrender() override;
};
}

