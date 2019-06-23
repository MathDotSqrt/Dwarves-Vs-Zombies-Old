#pragma once
#include "System.h"
#include "entt.hpp"
#include "Scene.h"

class BasicRenderSystem : public System {
private:
	Scene *scene;

public:
	BasicRenderSystem(Scene *scene, int priority);
	~BasicRenderSystem();

	void addedToEngine(entt::registry &engine);
	void removedFromEngine(entt::registry &engine);
	void update(entt::registry &engine, float delta);
};

