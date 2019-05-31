#include "TestState.h"
#include "ExampleState.h"
#include "macrologger.h"

bool test = false;

TestState::~TestState()
{
}

void TestState::init() {
	LOG_INFO("TestState: init()");
	this->m_isInitalized = true;
}

void TestState::cleanUp() {
	LOG_INFO("TestState: cleanUp()");

}

void TestState::entered() {
	LOG_INFO("TestState: entered()");
}

void TestState::leaving() {
	LOG_INFO("TestState: leaving()");

}

void TestState::obscuring() {
	LOG_INFO("TestState: obscured()");

}

void TestState::revealing() {
	LOG_INFO("TestState: revealing()");

}

void TestState::update(float delta) {
	LOG_INFO("TestState: update(%lf)", delta * 60);

}
