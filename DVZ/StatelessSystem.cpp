#include "StatelessSystem.h"
#include <chrono>
#include "Engine.h"

using namespace std::chrono_literals;

StatelessSystem::StatelessSystem(function_ptr ptr, FrameTime interval) : ptr(ptr), intervalTime(interval), currentTimeDuration(0) {

}

bool StatelessSystem::operator==(const StatelessSystem &other) {
	return ptr == other.ptr && intervalTime == other.intervalTime;
}

void StatelessSystem::update(Engine &engine, float delta) {
	currentTimeDuration += FrameTime(delta);

	if (currentTimeDuration >= intervalTime) {
		currentTimeDuration = FrameTime(0s);

		ptr(engine, delta);
	}
}
