#include "HandleVoxelCollision.h"

constexpr float EPSILON = 0.00f;
constexpr float MAX_NEW_VEL = 55.0f;

float calc_target_vel(float target_pos, float current_pos, float delta_time) {
	const float new_vel = (target_pos - current_pos) / delta_time;
	return  new_vel;
}

glm::vec3
handle_collision(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta_time) {
	//const auto &aabb = collision.aabb;
	//const auto &sample = collision.sample;
	//const auto max = pos + aabb.getMax();
	//const auto min = pos + aabb.getMin();

	//if (sample.px.has_value()) {
	//	vel.x = calc_target_vel(sample.px->first, max.x, delta_time);
	//}
	//else if(sample.nx.has_value()){
	//	vel.x = calc_target_vel(sample.nx->first, min.x, delta_time);
	//}

	//if (sample.py.has_value()) {
	//	vel.y = calc_target_vel(sample.py->first, max.y, delta_time);
	//}
	//else if (sample.ny.has_value()) {
	//	vel.y = calc_target_vel(sample.ny->first, min.y, delta_time);
	//}

	//if (sample.pz.has_value()) {
	//	vel.z = calc_target_vel(sample.pz->first, max.z, delta_time);
	//}
	//else if (sample.nz.has_value()) {
	//	vel.z = calc_target_vel(sample.nz->first, min.z, delta_time);
	//}


	return vel;
}

