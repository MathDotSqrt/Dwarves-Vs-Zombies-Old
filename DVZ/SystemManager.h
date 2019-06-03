#pragma once
#include <entt.hpp>
#include <list>
#include "System.h"


using namespace std;

class SystemManager {
private:
	list<System*> systems;

public:
	SystemManager();
	~SystemManager();

	void addSystem(entt::registry &engine, System *system);
	void removeSystem(entt::registry &engine, System *system);

	void updateSystems(entt::registry &engine, float delta);

	void deleteAllSystems(entt::registry &engine);
};

