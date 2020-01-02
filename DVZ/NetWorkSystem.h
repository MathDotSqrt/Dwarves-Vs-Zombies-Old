#pragma once
#include "System.h"


class NetWorkSystem : public System {
private:
	
public:
	NetWorkSystem(int priority);
	~NetWorkSystem();

	void addedToEngine(Engine * engine);
	void removedFromEngine(Engine * engine);
	void update(Engine * engine, float delta);
};

