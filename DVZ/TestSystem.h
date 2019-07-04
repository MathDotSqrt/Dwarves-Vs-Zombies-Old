#pragma once
#include "System.h"
#include <entt.hpp>


class TestSystem : public System {
public:
	TestSystem(int priority);
	~TestSystem();

	void addedToEngine(Engine * engine) override;
	void removedFromEngine(Engine * engine) override;
	void update(Engine * engine, float delta) override;
};

