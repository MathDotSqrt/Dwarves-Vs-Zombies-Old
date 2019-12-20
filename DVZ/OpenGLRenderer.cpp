#include "OpenGLRenderer.h"
#include "Window.h"
#include "ShaderSet.h"
#include "preamble.glsl"
#include "glm.hpp"
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>
#include "ChunkRenderDataManager.h"
#include "Timer.h"
#include "render_functions.h"
#include "QuadGeometry.h"
using namespace Graphics;

MaterialID ColorMaterial::type = MaterialID::COLOR_MATERIAL_ID;
MaterialID NormalMaterial::type = MaterialID::NORMAL_MAERIAL_ID;
MaterialID BasicLitMaterial::type = MaterialID::BASIC_LIT_MATERIAL_ID;
MaterialID TextureMaterial::type = MaterialID::TEXTURE_MATERIAL_ID;
MaterialID BlockMaterial::type = MaterialID::BLOCK_MATERIAL_ID;

OpenGLRenderer::OpenGLRenderer() : inverse(Window::getWidth(), Window::getHeight()), vbo(GL_ARRAY_BUFFER) {
	start = Window::getTime();
}


OpenGLRenderer::~OpenGLRenderer() {
}

void OpenGLRenderer::init(Scene *scene) {
	LOG_RENDER("Init");
	this->scene = scene;
	
	glClearColor(.4f, .4f, .4f, 1);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_CW);

	GLbyte quad_array[]{
		-1, -1,
		1, -1,
		-1,  1,
		-1,  1,
		1, -1,
		1,  1,
	};
	quad.bind();
	vbo.bind();
	vbo.bufferData(sizeof(quad_array), quad_array, GL_STATIC_DRAW);
	quad.bufferInterleavedData(vbo, Attrib<POSITION_ATTRIB_LOCATION, glm::i8vec2>());
	vbo.unbind();
	quad.unbind();

	Scene::Camera camera = {
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 1, 0),
		70 * 3.14159f / 180.0f,
		(float)Window::getWidth() / Window::getHeight(),
		.1f,
		1000
	};

	unsigned int cameraID = this->scene->createCameraInstance(camera);
	this->scene->setMainCamera(cameraID);

	Window::addResizeCallback(this);
	resize(Window::getWidth(), Window::getHeight());
	window_width = Window::getWidth();
	window_height = Window::getHeight();
}

void OpenGLRenderer::resize(int newWidth, int newHeight) {
	LOG_RENDER("Resize (%d, %d)", newWidth, newHeight);
	Scene::Camera *camera = &scene->cameraCache[scene->getMainCameraID()];
	camera->aspect = (float)newWidth / newHeight;

	this->perspectiveProjection = glm::perspective(camera->fov, camera->aspect, camera->near, camera->far);
	glViewport(0, 0, newWidth, newHeight);
}

void OpenGLRenderer::prerender() {
	sortedRenderStateKeys.clear();



	for (unsigned int instanceID : scene->instanceCache) {
		Scene::Instance instance = scene->instanceCache[instanceID];
		Scene::Mesh &mesh = scene->meshCache[instance.meshID];
		MaterialID matID = mesh.typeID;

		

		RenderStateKey key(ViewPort::DEFAULT, ViewPortLayer::DEFAULT, BlendType::OPAQUE, matID, instanceID);
		sortedRenderStateKeys.push_back(key);
	}
	std::sort(sortedRenderStateKeys.begin(), sortedRenderStateKeys.end());

	duration = Window::getTime() - start;
}

void OpenGLRenderer::render(Voxel::ChunkRenderDataManager *manager) {
	
	
	Scene::Camera *camera = &scene->cameraCache[scene->getMainCameraID()];
	glm::mat4 view = glm::lookAt(camera->eye, camera->eye + camera->target, camera->up);
	glm::mat4 vp = perspectiveProjection * view;
	glm::vec3 camera_position = camera->eye;

	ShaderVariables::camera_pos = camera->eye;
	ShaderVariables::p = perspectiveProjection;
	ShaderVariables::v = glm::lookAt(camera->eye, camera->eye + camera->target, camera->up);
	ShaderVariables::vp = ShaderVariables::p * ShaderVariables::v;

	int index = 0;
	inverse.bind();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	index = renderBasic(index, vp);
	index = renderNormal(index, vp);
	index = renderBasicLit(index, camera_position, vp);
	renderChunks(manager, camera_position, vp);
	inverse.unbind();

	renderPostProcess();
}

void OpenGLRenderer::postrender() {
	
}

int OpenGLRenderer::renderBasic(int startIndex, glm::mat4 vp) {

	if (!this->isValidState(startIndex, MaterialID::COLOR_MATERIAL_ID)) {
		return startIndex;
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::mat4 ident = glm::identity<glm::mat4>();
	Shader::GLSLProgram *shader = Shader::getShaderSet({ "basic_shader.vert", "basic_shader.frag" });
	shader->use();
	int index = startIndex;
	do {
		RenderStateKey key = sortedRenderStateKeys[index];
		Scene::Instance *instance = &scene->instanceCache[key.getValue()];
		Scene::Mesh *mesh = &scene->meshCache[instance->meshID];
		Scene::Transformation *transformation = &scene->transformationCache[instance->transformationID];

		glm::quat rot(transformation->rotation);
		glm::vec3 axis = glm::axis(rot);

		float angle = glm::angle(rot);

		glm::mat4 model;
		model = glm::translate(ident, transformation->position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, transformation->scale);

		shader->setUniformMat4("MVP", vp * model);
		
		ColorMaterial *material = &scene->colorMaterialCache[mesh->materialInstanceID];
		shader->setUniform3f("color", material->color);

		mesh->vao.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);

		index++;
	} while (this->isValidState(index, MaterialID::COLOR_MATERIAL_ID));

	shader->end();
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return index;
}

int OpenGLRenderer::renderNormal(int startIndex, glm::mat4 vp) {
	if (!this->isValidState(startIndex, MaterialID::NORMAL_MAERIAL_ID)) {
		return startIndex;
	}


	glm::mat4 ident = glm::identity<glm::mat4>();
	Shader::GLSLProgram *shader = Shader::getShaderSet({"normal_shader.vert", "normal_shader.frag"});
	shader->use();

	int index = startIndex;
	do {
		RenderStateKey state = sortedRenderStateKeys[index];

		Scene::Instance *instance = &scene->instanceCache[state.getValue()];
		Scene::Mesh *mesh = &scene->meshCache[instance->meshID];
		Scene::Transformation *transformation = &scene->transformationCache[instance->transformationID];

		glm::quat rot(transformation->rotation);
		glm::vec3 axis = glm::axis(rot);

		float angle = glm::angle(rot);

		glm::mat4 model;
		model = glm::translate(ident, transformation->position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, transformation->scale);

		glm::mat3 mat(model);
		shader->setUniformMat3("inverseTransposeMatrix", glm::inverse(mat), true);
		shader->setUniformMat4("MVP", vp * model);

		mesh->vao.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);

		index++;
	} while (this->isValidState(index, MaterialID::NORMAL_MAERIAL_ID));

	shader->end();
	glBindVertexArray(0);

	return index;
}

int OpenGLRenderer::renderBasicLit(int startIndex, glm::vec3 camera_position, glm::mat4 vp) {
	if (!this->isValidState(startIndex, MaterialID::BASIC_LIT_MATERIAL_ID)) {
		return startIndex;
	}

	glm::mat4 ident = glm::identity<glm::mat4>();
	Shader::GLSLProgram *shader = Shader::getShaderSet({ "basic_lit_shader.vert", "basic_lit_shader.frag" });
	shader->use();

	shader->setUniform3f("camera_pos", camera_position);
	shader->setUniformMat4("VP", vp);

	Scene::PointLight &point = this->scene->pointLightCache[0];
	shader->setUniform3f("point_light_position", point.position);
	shader->setUniform3f("point_light_color", point.color);
	shader->setUniform1f("point_light_intensity", point.intensity);

	int index = startIndex;
	do {
		RenderStateKey state = sortedRenderStateKeys[index];

		Scene::Instance *instance = &scene->instanceCache[state.getValue()];
		Scene::Mesh *mesh = &scene->meshCache[instance->meshID];
		Scene::Transformation *transformation = &scene->transformationCache[instance->transformationID];

		glm::quat rot(transformation->rotation);
		glm::vec3 axis = glm::axis(rot);

		float angle = glm::angle(rot);

		glm::mat4 model;
		model = glm::translate(ident, transformation->position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, transformation->scale);

		glm::mat3 mat(model);
		shader->setUniformMat3("inverseTransposeMatrix", glm::inverse(mat), true);
		shader->setUniformMat4("M", model);

		BasicLitMaterial &material = scene->basicLitMaterialCache[mesh->materialInstanceID];
		shader->setUniform3f("diffuse_color", material.diffuseColor);
		shader->setUniform3f("specular_color", material.specularColor);
		shader->setUniform1f("shinyness", material.shinyness);


		mesh->vao.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);

		index++;
	} while (this->isValidState(index, MaterialID::BASIC_LIT_MATERIAL_ID));

	shader->end();
	glBindVertexArray(0);

	return index;
}

int OpenGLRenderer::renderBasicBlock(int startIndex, glm::vec3 camera_position, glm::mat4 vp) {
	if (!this->isValidState(startIndex, MaterialID::BLOCK_MATERIAL_ID)) {
		return startIndex;
	}

	glm::mat4 ident = glm::identity<glm::mat4>();
	Shader::GLSLProgram *shader = Shader::getShaderSet({ "chunk_shader.vert", "chunk_shader.frag" });
	shader->use();


	shader->setUniform3f("camera_pos", camera_position);
	shader->setUniformMat4("VP", vp);

	Scene::PointLight &point = this->scene->pointLightCache[0];
	shader->setUniform3f("point_light_position", point.position);
	shader->setUniform3f("point_light_color", point.color);
	shader->setUniform1f("point_light_intensity", point.intensity);

	int index = startIndex;
	do {
		RenderStateKey state = sortedRenderStateKeys[index];

		Scene::Instance *instance = &scene->instanceCache[state.getValue()];
		Scene::Mesh *mesh = &scene->meshCache[instance->meshID];
		Scene::Transformation *transformation = &scene->transformationCache[instance->transformationID];

		glm::quat rot(transformation->rotation);
		glm::vec3 axis = glm::axis(rot);

		float angle = glm::angle(rot);

		glm::mat4 model;
		model = glm::translate(ident, transformation->position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, transformation->scale);

		glm::mat3 mat(model);
		shader->setUniformMat3("inverseTransposeMatrix", glm::inverse(mat), true);
		shader->setUniformMat4("M", model);

		BlockMaterial &material = scene->blockMaterialCache[mesh->materialInstanceID];
		shader->setUniform3f("specular_color", material.specularColor);
		shader->setUniform1f("shinyness", material.shinyness);


		mesh->vao.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glEnableVertexAttribArray(COLOR_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);

		index++;
	} while (this->isValidState(index, MaterialID::BLOCK_MATERIAL_ID));

	shader->end();
	glBindVertexArray(0);

	return index;
}

void OpenGLRenderer::renderChunks(Voxel::ChunkRenderDataManager *manager, glm::vec3 camera_position, glm::mat4 vp) {
	Util::Performance::Timer chunks("RenderChunks");

	const Graphics::BlockMaterial chunkMat = { {.95f, .7f, .8f}, 30 };
	const glm::vec3 CHUNK_SCALE(Voxel::CHUNK_RENDER_WIDTH_X, Voxel::CHUNK_RENDER_WIDTH_Y, Voxel::CHUNK_RENDER_WIDTH_Z);

	Shader::GLSLProgram *shader = Shader::getShaderSet({ "new_chunk_shader.vert", "new_chunk_shader.frag" });
	shader->use();

	shader->setUniform3f("fog.color", glm::vec3(.13, .13, .13));
	shader->setUniform1f("fog.start_dist", 0);
	shader->setUniform1f("fog.attenuation", 0.004f);

	shader->setUniform3f("dirLight.dir", glm::vec3(1, -1, 3));
	shader->setUniform3f("dirLight.color", glm::vec3(1, 1, 1));

	shader->setUniform3f("ambient.color", glm::vec3(1, 1, 1));
	shader->setUniform1f("ambient.intensity", .2f);

	shader->setUniform3f("camera_pos", camera_position);
	shader->setUniformMat4("VP", vp);

	Scene::PointLight &point = this->scene->pointLightCache[0];

	//this->chunkTex.bindActiveTexture(0);
	this->debugTEX.bindActiveTexture(0);

	for (auto iterator = manager->begin(); iterator != manager->end(); iterator++) {

		auto data = *iterator;
		
		shader->setUniform3f("chunk_pos", data.pos * CHUNK_SCALE);
		shader->setUniform1f("time", getShaderTime() - data.startTime);

		glBindVertexArray(data.vaoID);
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glEnableVertexAttribArray(BLOCK_TEXCOORD_ATTRIB_LOCATION);
 		glDrawElements(GL_TRIANGLES, (GLsizei)data.indexCount, GL_UNSIGNED_INT, 0);
	}

	shader->end();
	glBindVertexArray(0);
}

void OpenGLRenderer::renderPostProcess() {
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, window_width, window_height);

	Shader::GLSLProgram *program = Shader::getShaderSet({"frame_buffer_shader.vert", "frame_buffer_shader.frag"});
	program->use();

	inverse.getColorAttachment().bindActiveTexture(0);
	program->setUniform1f("time", getShaderTime());
	quad.bind();
	glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	quad.unbind();

	program->end();

}

bool OpenGLRenderer::isValidState(int sortedStateKeyIndex, MaterialID typeID) {
	return sortedStateKeyIndex < sortedRenderStateKeys.size()
		&& typeID == sortedRenderStateKeys[sortedStateKeyIndex].getMaterialID();
}

