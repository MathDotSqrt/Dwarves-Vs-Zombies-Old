#pragma once
#include "IntervalSystem.h"
class ShaderUpdaterSystem : public IntervalSystem
{
public:
	ShaderUpdaterSystem(float interval, int priority);
	~ShaderUpdaterSystem();

	void addedToEngine(Engine *engine);
	void removedFromEngine(Engine *engine);
	void updateInterval(Engine *engine, float delta);
};

