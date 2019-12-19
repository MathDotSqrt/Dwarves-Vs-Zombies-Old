#pragma once
#include "IRenderer.h"
#include "TEX.h"
#include "FBO.h"
#include "RenderStateKey.h"

namespace Graphics {

class OpenGLRenderer : public IRenderer {
private:


	std::vector<RenderStateKey> sortedRenderStateKeys;
	glm::mat4 perspectiveProjection;


	bool isValidState(int sortedStateKeyIndex, MaterialID matID);

	
	Graphics::TEX chunkTex = Graphics::TEX::Builder("terrain.png")
		.rgba()
		.nearest()
		.clampToEdge()
		.repeat()
		.borderColor(0, 1, 0, 0)
		.mipmapLinear()
		.buildTextureAtlasArray(16, 16);

	Graphics::TEX debugTEX = Graphics::TEX::Builder("WorldTerrain2.png")
		.rgb()
		.linear()
		.mipmapLinear()
		.repeat()
		.buildTextureAtlasArray(2, 2);

	double start;
	double duration;

	
	int window_width = 0;
	int window_height = 0;
	
	FBO inverse;
	VAO quad;
	VBO vbo;


	int renderBasic(int startKeyIndex, glm::mat4 vp);
	int renderNormal(int startKeyIndex, glm::mat4 vp);
	int renderBasicLit(int startKeyIndex, glm::vec3 camera_position, glm::mat4 vp);
	int renderBasicBlock(int startKeyIndex, glm::vec3 camera_position, glm::mat4 vp);
	void renderChunks(Voxel::ChunkRenderDataManager *manager, glm::vec3 camera_position, glm::mat4 vp);
	void renderPostProcess();

	float getShaderTime() {
		return (float)duration;
	}

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

