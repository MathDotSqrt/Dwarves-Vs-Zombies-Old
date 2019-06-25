#pragma once
#include <GL/glew.h>
#include <unordered_map>
#include <vector>
#include "Scene.h"
#include "Shader.h"

namespace Graphics {
	namespace OpenGLRenderer {
		void init();
		void resize(int newWidth, int newHeight);

		void preRender();
		void render(Scene *scene);
	};
};

//
//using namespace std;
//
//namespace Graphics{
//
//class OpenGLRenderer {
//private:
//
//public:
//	OpenGLRenderer();
//	~OpenGLRenderer();
//
//	void init();
//	//void resize(int newWidth, int newHeight);
//	void render(Scene *scene);
//
//
//};
//}
