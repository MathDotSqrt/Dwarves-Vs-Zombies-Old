#include "SystemManager.h"
#include <algorithm>


SystemManager::SystemManager(){

}


SystemManager::~SystemManager() {
}

void SystemManager::addSystem(entt::registry &engine, System *system) {
	if (find(this->systems.begin(), this->systems.end(), system) == this->systems.end()) {
		this->systems.insert(std::lower_bound(this->systems.begin(), this->systems.end(), system), system);

		system->addedToEngine(engine);
	}
}

void SystemManager::removeSystem(entt::registry &engine, System *system) {
	if (find(this->systems.begin(), this->systems.end(), system) != this->systems.end()) {
		system->removedFromEngine(engine);
		this->systems.remove(system);
	}
}

void SystemManager::updateSystems(entt::registry &engine, float delta) {
	for (System *system : this->systems) {
		system->update(engine, delta);
	}
}

void SystemManager::deleteAllActiveSystems(entt::registry &engine) {
	for (System *system : this->systems) {
		this->removeSystem(engine, system);

		delete system;
	}

	this->systems.clear();
}