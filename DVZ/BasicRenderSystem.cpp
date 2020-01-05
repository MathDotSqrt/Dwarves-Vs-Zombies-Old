#include "BasicRenderSystem.h"
#include "Engine.h"


using namespace glm;

BasicRenderSystem::BasicRenderSystem(int priority) : System(priority) {
}


BasicRenderSystem::~BasicRenderSystem()
{
}

void BasicRenderSystem::addedToEngine(Engine * engine) {
	LOG_SYSTEM("Added to engine");
	
}

void BasicRenderSystem::removedFromEngine(Engine * engine) {
	LOG_SYSTEM("Removed from engine");
}

void BasicRenderSystem::update(Engine * engine, float delta) {
	
	
}
