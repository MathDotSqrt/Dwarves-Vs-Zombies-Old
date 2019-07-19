#include "TestSystem.h"
#include "Engine.h"
#include "macrologger.h"


TestSystem::TestSystem(int priority) : System(priority){

}


TestSystem::~TestSystem() {
}

void TestSystem::addedToEngine(Engine * engine) {
	LOG_SYSTEM("Added to engine");
}

void TestSystem::removedFromEngine(Engine * engine) {
	LOG_SYSTEM("Removed from engine");
}

void TestSystem::update(Engine * engine, float delta) {

}