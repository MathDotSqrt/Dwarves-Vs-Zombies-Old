#include "TestState.h"
#include "ExampleState.h"
#include "macrologger.h"
#include "TestSystem.h"

bool test = false;

TestState::~TestState()
{
	LOG_INFO("TOUCH ME");
}

void TestState::init(entt::registry &engine) {
	LOG_INFO("TestState: init()");
	this->m_isInitalized = true;

	
	this->systemManager->addSystem(engine, new TestSystem(0));
}

void TestState::cleanUp(entt::registry &engine) {
	LOG_INFO("TestState: cleanUp()");
	this->systemManager->deleteAllSystems(engine);
}

void TestState::entered(entt::registry &engine) {
	LOG_INFO("TestState: entered()");
}

void TestState::leaving(entt::registry &engine) {
	LOG_INFO("TestState: leaving()");

}

void TestState::obscuring(entt::registry &engine) {
	LOG_INFO("TestState: obscured()");

}

void TestState::revealing(entt::registry &engine) {
	LOG_INFO("TestState: revealing()");

}

void TestState::update(entt::registry &engine, float delta) {
	this->systemManager->updateSystems(engine, delta);
}
