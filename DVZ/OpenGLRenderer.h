#pragma once
#include <GL/glew.h>
#include <unordered_map>
#include <vector>
#include "Scene.h"
#include "Shader.h"


using namespace std;

namespace Graphics{

class OpenGLRenderer {
private:
	unordered_map<int, vector<int>> renderables;

public:
	OpenGLRenderer();
	~OpenGLRenderer();

	void init();
	void resize(int width, int height);
	void render(Scene *scene);


};
}
