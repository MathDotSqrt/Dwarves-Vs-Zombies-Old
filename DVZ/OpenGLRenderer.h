#pragma once
#include "IRenderer.h"
#include "TEX.h"
#include "FBO.h"
#include "Common.h"
namespace Graphics {

class OpenGLRenderer : public IRenderer {
private:
	typedef unsigned long long RenderStateKey;
	struct _RenderStateKey {
		uint64 key;

		void setMaterialID();
		void setInstanceID();
		MaterialID getMaterialID() const;
		uint32 getInstanceID() const;
	};

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
	void renderChunks(Voxel::ChunkManager *manager, glm::vec3 camera_position, glm::mat4 vp);
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
	void render(Voxel::ChunkManager *manager) override;
	void postrender() override;
};
}

