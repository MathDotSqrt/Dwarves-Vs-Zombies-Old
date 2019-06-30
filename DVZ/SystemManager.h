#pragma once
#include <entt.hpp>
#include <list>
#include "System.h"

class SystemManager {
private:
	std::list<System*> systems;

public:
	SystemManager();
	~SystemManager();

	void addSystem(entt::registry &engine, System *system);
	void removeSystem(entt::registry &engine, System *system);

	void updateSystems(entt::registry &engine, float delta);

	void deleteAllActiveSystems(entt::registry &engine);
};

