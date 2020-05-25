#include "Components.h"
Component::RenderInstance::RenderInstance(uint32 instanceID) : instanceID(instanceID), offset(0){}
Component::RenderInstance::RenderInstance(uint32 instanceID, glm::vec3 offset) : instanceID(instanceID), offset(offset) {}