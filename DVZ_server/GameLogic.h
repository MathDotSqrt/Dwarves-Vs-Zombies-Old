#pragma once
#include "EntityAdmin.h"

namespace GameLogic {
	void input_system(EntityAdmin &admin, float delta);
	void movement_system(EntityAdmin &admin, float delta);
	void afk_system(EntityAdmin &admin, float delta);
	void voxel_system(EntityAdmin &admin, float delta);
}