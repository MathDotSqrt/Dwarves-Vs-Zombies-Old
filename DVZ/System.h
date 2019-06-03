#pragma once
#include <entt.hpp>


class System {
private:
	unsigned int priority;

public:
	System(unsigned int priority);
	~System();

	virtual void addedToEngine(entt::registry &engine) = 0;
	virtual void removedFromEngine(entt::registry &engine) = 0;
	virtual void update(entt::registry &engine, float delta) = 0;

	bool operator < (const System *system) const {
		return this->priority < system->priority;
	}

	bool operator > (const System *system) const {
		return this->priority > system->priority;
	}
};

