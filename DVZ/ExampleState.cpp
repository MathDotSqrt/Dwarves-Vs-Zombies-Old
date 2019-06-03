#include "ExampleState.h"
#include "TestState.h"

ExampleState::~ExampleState()
{
	LOG_INFO("TOUCH ME");
}

void ExampleState::init(entt::registry &engine) {
	LOG_INFO("ExampleState: init()");
	this->m_isInitalized = true;

}

void ExampleState::cleanUp(entt::registry &engine) {
	LOG_INFO("ExampleState: cleanUp()");

}

void ExampleState::entered(entt::registry &engine) {
	LOG_INFO("ExampleState: entered()");

}

void ExampleState::leaving(entt::registry &engine) {
	LOG_INFO("ExampleState: leaving()");

}

void ExampleState::obscuring(entt::registry &engine) {
	LOG_INFO("ExampleState: obscured()");

}

void ExampleState::revealing(entt::registry &engine) {
	LOG_INFO("ExampleState: revealing()");

}

void ExampleState::update(entt::registry &engine, float delta) {
}
