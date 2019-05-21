#include <entt.hpp>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
struct position {
	float x, y, z;
};

struct velocity {
	float dx, dy, dz;
};

int main() {

	entt::registry reg;
	auto view = reg.view<position, velocity>();
	reg.destroy(view.begin(), view.end());

	reg.create<position>();
	
}