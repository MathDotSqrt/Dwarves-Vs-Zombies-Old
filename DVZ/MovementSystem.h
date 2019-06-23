#pragma once
#include "System.h"
class MovementSystem :
	public System
{
public:
	MovementSystem(int priority);
	~MovementSystem();

	void addedToEngine(entt::registry &engine);
	void removedFromEngine(entt::registry &engine);
	void update(entt::registry &engine, float delta);
};

