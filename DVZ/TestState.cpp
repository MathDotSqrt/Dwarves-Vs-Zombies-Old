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

	if (test == false) {
		test = true;
		gsm->enterState(GameState::getInstance<ExampleState>(gsm));
	}

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

void TestState::update() {
	LOG_INFO("TestState: update()");

}
