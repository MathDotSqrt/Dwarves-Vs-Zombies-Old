#pragma once
#include "entt.hpp"


class Engine;

class System {
private:
	unsigned int priority;

public:
	System(unsigned int priority);
	virtual ~System();

	virtual void addedToEngine(Engine *engine) = 0;
	virtual void removedFromEngine(Engine *engine) = 0;
	virtual void update(Engine *engine, float delta) = 0;

	bool operator < (const System *system) const {
		return this->priority < system->priority;
	}

	bool operator > (const System *system) const {
		return this->priority > system->priority;
	}

	struct classcomp {
		bool operator()(const System *lhs, const System *rhs) const { 
			return (lhs->priority < rhs->priority); 
		}
	};

	unsigned int getPriority();
};

