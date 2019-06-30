#pragma once
#include "System.h"
class InputSystem : public System
{
public:
	InputSystem(int priority);
	~InputSystem();

	void addedToEngine(entt::registry &engine);
	void removedFromEngine(entt::registry &engine);
	void update(entt::registry &engine, float delta);
};

