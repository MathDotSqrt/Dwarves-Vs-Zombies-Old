#pragma once
#include "System.h"
class IntervalSystem : public System {
private:
	float interval;
	float lastTime;


public:
	IntervalSystem(float interval, int priority);
	virtual ~IntervalSystem();
	virtual void updateInterval(Engine *engine, float delta) = 0;

	
	void update(Engine *engine, float delta);
	float getInterval();
};

