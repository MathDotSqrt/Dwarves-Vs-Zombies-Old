#pragma once
#include "System.h"
#include "entt.hpp"

class BasicRenderSystem : public System
{
public:
	BasicRenderSystem(int priority);
	~BasicRenderSystem();

	void addedToEngine(entt::registry &engine);
	void removedFromEngine(entt::registry &engine);
	void update(entt::registry &engine, float delta);
};

