#include "ShaderUpdaterSystem.h"
#include "macrologger.h"

#include "ShaderSet.h"
#include "windows.h"
#include "fileapi.h"


ShaderUpdaterSystem::ShaderUpdaterSystem(float interval, int priority) : IntervalSystem(interval, priority) {

}


ShaderUpdaterSystem::~ShaderUpdaterSystem() {
}

void ShaderUpdaterSystem::addedToEngine(Engine *engine) {
	LOG_SYSTEM("added to engine");
}

void ShaderUpdaterSystem::removedFromEngine(Engine *engine) {
	LOG_SYSTEM("removed from engine");
}

void ShaderUpdaterSystem::updateInterval(Engine *engine, float delta) {
	
}
