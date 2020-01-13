#include "StatelessSystem.h"
#include <chrono>


StatelessSystem::StatelessSystem(function_ptr ptr, FrameTime interval) : 
	ptr(ptr), 
	intervalTime(interval), 
	currentTimeDuration(0) {

}


StatelessSystem::~StatelessSystem() {

}

bool StatelessSystem::operator==(const StatelessSystem &other) {
	return ptr == other.ptr && intervalTime == other.intervalTime;
}

void StatelessSystem::update(EntityAdmin &admin, float delta) {
	currentTimeDuration += FrameTime(delta);
	
	
	if (currentTimeDuration >= intervalTime) {
		currentTimeDuration = FrameTime(0);

		ptr(admin, delta);
	}
}