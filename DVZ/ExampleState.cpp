#include "ExampleState.h"
#include "TestState.h"

ExampleState::~ExampleState()
{
}

void ExampleState::init() {
	LOG_INFO("ExampleState: init()");
	this->m_isInitalized = true;

}

void ExampleState::cleanUp() {
	LOG_INFO("ExampleState: cleanUp()");

}

void ExampleState::entered() {
	LOG_INFO("ExampleState: entered()");
	gsm->stackState(GameState::getInstance<TestState>(gsm));

}

void ExampleState::leaving() {
	LOG_INFO("ExampleState: leaving()");

}

void ExampleState::obscuring() {
	LOG_INFO("ExampleState: obscured()");

}

void ExampleState::revealing() {
	LOG_INFO("ExampleState: revealing()");

}

void ExampleState::update(float delta) {
	LOG_INFO("ExampleState: update()");
}
