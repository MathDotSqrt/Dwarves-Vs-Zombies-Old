#pragma once
#include "System.h"
#include <entt.hpp>

class TestSystem : public System {
public:
	TestSystem(int priority);
	~TestSystem();

	void addedToEngine(entt::registry &engine) override;
	void removedFromEngine(entt::registry &engine) override;
	void update(entt::registry &engine, float delta) override;
};

