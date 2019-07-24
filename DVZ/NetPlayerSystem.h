#pragma once
#include "IntervalSystem.h"
class NetPlayerSystem : public IntervalSystem
{
public:
	NetPlayerSystem(float interval, int priority);
	~NetPlayerSystem();

	void addedToEngine(Engine *engine);
	void removedFromEngine(Engine *engine);
	void updateInterval(Engine *engine, float delta);
};

