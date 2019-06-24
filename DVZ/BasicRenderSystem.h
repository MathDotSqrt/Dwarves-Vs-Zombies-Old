#pragma once
#include "System.h"
#include "entt.hpp"
#include "Scene.h"

class BasicRenderSystem : public System {
private:
	Graphics::Scene *scene;

public:
	BasicRenderSystem(Graphics::Scene *scene, int priority);
	~BasicRenderSystem();

	void addedToEngine(entt::registry &engine);
	void removedFromEngine(entt::registry &engine);
	void update(entt::registry &engine, float delta);
};

