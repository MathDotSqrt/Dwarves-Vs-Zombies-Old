#pragma once
#include "IRenderer.h"

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
public:
	OpenGLRenderer();
	~OpenGLRenderer();

	void init(Scene *scene) override;
	void resize(int newWidth, int newHeight) override;
	void prerender() override;
	void render() override;
};
}

