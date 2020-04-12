#pragma once
#include "glm.hpp"
#include "SharedComponents.h"

glm::vec3 
handle_collision(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta_time);
