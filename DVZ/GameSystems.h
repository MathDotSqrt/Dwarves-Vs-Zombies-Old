
class Engine;

namespace GameSystem {
	void movement_system(Engine &engine, float delta);
	void input_system(Engine &engine, float delta);
	void render_system(Engine &engine, float delta);
	void netword_system(Engine &engine, float delta);
	void send_packet_system(Engine &engine, float delta);
	void shader_update_system(Engine &engine, float delta);
	void voxel_system(Engine &engine, float delta);
}