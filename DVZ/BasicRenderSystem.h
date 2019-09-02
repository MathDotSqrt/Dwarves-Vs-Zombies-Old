#pragma once
#include "System.h"
#include "entt.hpp"
#include "Scene.h"

class BasicRenderSystem : public System {
private:

public:
	BasicRenderSystem(int priority);
	~BasicRenderSystem();

	void addedToEngine(Engine *engine);
	void removedFromEngine(Engine *engine);
	void update(Engine *engine, float delta);
};

