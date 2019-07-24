#include "NetPlayerSystem.h"
#include "macrologger.h"


NetPlayerSystem::NetPlayerSystem(float interval, int priority) : IntervalSystem(interval, priority){
}


NetPlayerSystem::~NetPlayerSystem() {
}

void NetPlayerSystem::addedToEngine(Engine *engine) {

}

void NetPlayerSystem::removedFromEngine(Engine *engine) {

}



void NetPlayerSystem::updateInterval(Engine *engine, float delta) {
	LOG_SYSTEM("NET PLAYER UPDATE %f", delta);
}

