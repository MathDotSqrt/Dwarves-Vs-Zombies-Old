#pragma once
#include "EntityAdmin.h"

namespace System {
	void net_update(EntityAdmin &admin, float delta);
	void net_disconnect(EntityAdmin &admin, float delta);
	void net_broadcast(EntityAdmin &admin, float delta);
	void net_voxel(EntityAdmin &admin, float delta);
}
