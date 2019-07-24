#include "IntervalSystem.h"



IntervalSystem::IntervalSystem(float interval, int priority) : System(priority) {
	this->interval = interval;
	this->lastTime = 0;
}


IntervalSystem::~IntervalSystem() {

}

void IntervalSystem::update(Engine *engine, float delta) {
	lastTime += delta;

	if (this->lastTime >= this->interval) {
		this->updateInterval(engine, lastTime);
		//TODO figure out if I want to perserve the average update to match interval or to
		//just miss updates when a frame takes longer than the interval time 
		//this->lastTime -= this->interval;
		this->lastTime = 0;
	}
}


float IntervalSystem::getInterval(){
	return this->interval;
}
