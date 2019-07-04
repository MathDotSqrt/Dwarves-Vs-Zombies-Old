#pragma once
#include "System.h"
class MovementSystem :
	public System
{
public:
	MovementSystem(int priority);
	~MovementSystem();

	void addedToEngine(Engine * engine);
	void removedFromEngine(Engine * engine);
	void update(Engine * engine, float delta);
};

