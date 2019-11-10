#pragma once
#include "IRenderer.h"
#include "TEX.h"

namespace Graphics {

class OpenGLRenderer : public IRenderer {
private:
	typedef unsigned long long RenderStateKey;
	struct RenderState {
		MaterialID materialID;
		unsigned int instanceID;
	};

	std::vector<RenderStateKey> sortedRenderStateKeys;
	glm::mat4 perspectiveProjection;

	RenderStateKey createRenderStateKey(RenderState state);
	RenderState getRenderStateFromKey(RenderStateKey key);
	RenderState getRenderStateFromIndex(int sortedRenderStateKeyIndex);
	bool isValidState(int sortedStateKeyIndex, MaterialID matID);

	int renderBasic(int startKeyIndex, glm::mat4 vp);
	int renderNormal(int startKeyIndex, glm::mat4 vp);
	int renderBasicLit(int startKeyIndex, glm::vec3 camera_position, glm::mat4 vp);
	int renderBasicBlock(int startKeyIndex, glm::vec3 camera_position, glm::mat4 vp);


	Graphics::TEX chunkTex = Graphics::TEX::Builder("terrain.png")
		.rgb()
		.nearest()
		.clampToEdge()
		.borderColor(0, 1, 0, 0)
		.mipmapLinear()
		.buildTextureAtlasArray(16, 16);

	void renderChunks(Voxel::ChunkManager *manager, glm::vec3 camera_position, glm::mat4 v, glm::mat4 p);

public:
	OpenGLRenderer();
	~OpenGLRenderer();

	void init(Scene *scene) override;
	void resize(int newWidth, int newHeight) override;
	void prerender() override;
	void render(Voxel::ChunkManager *manager) override;
};
}

