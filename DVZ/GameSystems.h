
class Engine;

namespace System {

	void movement_system(Engine &engine, float delta);
	void friction_system(Engine &engine, float delta);

	void player_state_system(Engine &engine, float delta);
	void sprint_system(Engine &engine, float delta);

	void collision_test_system(Engine &engine, float delta);

	void input_system(Engine &engine, float delta);
	void voxel_system(Engine &engine, float delta);
	void render_system(Engine &engine, float delta);

	void netword_system(Engine &engine, float delta);
	void send_packet_system(Engine &engine, float delta);
	void shader_update_system(Engine &engine, float delta);
	void voxel_collision_system(Engine &engine, float delta);
}