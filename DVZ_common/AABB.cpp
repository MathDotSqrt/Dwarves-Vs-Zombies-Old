#include "AABB.h"

bool Physics::intersect(const Physics::AABB &a, const Physics::AABB &b) {
	return
		(a.min.x <= a.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= a.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= a.max.z && a.max.z >= b.min.z);
}