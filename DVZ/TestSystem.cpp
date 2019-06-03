#include "TestSystem.h"
#include "macrologger.h"


TestSystem::TestSystem(int priority) : System(priority){

}


TestSystem::~TestSystem() {
}

void TestSystem::addedToEngine(entt::registry &engine) {
	LOG_INFO("Added To Engine: TestSystem");
}

void TestSystem::removedFromEngine(entt::registry &engine) {
	LOG_INFO("Removed From Engine: TestSystem");
}

void TestSystem::update(entt::registry &engine, float delta) {

}