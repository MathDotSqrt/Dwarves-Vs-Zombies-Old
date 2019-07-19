#include "System.h"



System::System(unsigned int priority) : priority(priority) {

}


System::~System() {
}

unsigned int System::getPriority() {
	return this->priority;
}