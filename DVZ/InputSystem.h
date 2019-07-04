#pragma once
#include "System.h"
class InputSystem : public System
{
public:
	InputSystem(int priority);
	~InputSystem();

	void addedToEngine(Engine * engine);
	void removedFromEngine(Engine * engine);
	void update(Engine * engine, float delta);
};

