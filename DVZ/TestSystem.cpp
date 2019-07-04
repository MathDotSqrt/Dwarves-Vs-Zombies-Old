#include "TestSystem.h"
#include "Engine.h"
#include "macrologger.h"


TestSystem::TestSystem(int priority) : System(priority){

}


TestSystem::~TestSystem() {
}

void TestSystem::addedToEngine(Engine * engine) {
	LOG_INFO("Added To Engine: TestSystem");
}

void TestSystem::removedFromEngine(Engine * engine) {
	LOG_INFO("Removed From Engine: TestSystem");
}

void TestSystem::update(Engine * engine, float delta) {

}