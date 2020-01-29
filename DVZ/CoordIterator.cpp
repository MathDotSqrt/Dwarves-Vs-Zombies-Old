#include "CoordIterator.h"

using namespace Voxel;

CoordIterator::CoordIterator(glm::i32vec3 min, glm::i32vec3 max) : next_coord(min), min(min), max(max) {

}


CoordIterator::~CoordIterator() {

}

glm::i32vec3 CoordIterator::next() {
	const auto current = next_coord;

	if (next_coord.x == max.x) {
		next_coord.x = min.x;
		if (next_coord.y == max.y) {
			next_coord.y = min.y;
			if (next_coord.z == max.z) {
				assert(false);
			}
			else {
				next_coord.z++;
			}
		}
		else {
			next_coord.y++;
		}
	}
	else {
		next_coord.x++;
	}

	return current;
}

constexpr int CoordIterator::length() const {
	const auto delta = (max - min) + glm::i32vec3(1);
	return delta.x * delta.y * delta.z;
}
