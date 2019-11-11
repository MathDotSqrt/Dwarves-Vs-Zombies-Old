#include "OpenGLRenderer.h"
#include "Window.h"
#include "ShaderSet.h"
#include "preamble.glsl"
#include "glm.hpp"
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>
#include "ChunkManager.h"
#include "Timer.h"

using namespace Graphics;

MaterialID ColorMaterial::type = MaterialID::COLOR_MATERIAL_ID;
MaterialID NormalMaterial::type = MaterialID::NORMAL_MAERIAL_ID;
MaterialID BasicLitMaterial::type = MaterialID::BASIC_LIT_MATERIAL_ID;
MaterialID TextureMaterial::type = MaterialID::TEXTURE_MATERIAL_ID;
MaterialID BlockMaterial::type = MaterialID::BLOCK_MATERIAL_ID;

OpenGLRenderer::OpenGLRenderer(){
}


OpenGLRenderer::~OpenGLRenderer() {
}

void OpenGLRenderer::init(Scene *scene) {
	LOG_RENDER("Init");
	this->scene = scene;
	
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

	//glClearColor(.1f, .4f, .7f, 1);
	glClearColor(.4f, .4f, .4f, 1);
	glEnable(GL_DEPTH_TEST);

	Window::addResizeCallback(this);
	this->resize(Window::getWidth(), Window::getHeight());
}

void OpenGLRenderer::resize(int newWidth, int newHeight) {
	LOG_RENDER("Resize (%d, %d)", newWidth, newHeight);
	Scene::Camera *camera = &scene->cameraCache[scene->getMainCameraID()];
	camera->aspect = (float)newWidth / newHeight;

	this->perspectiveProjection = glm::perspective(camera->fov, camera->aspect, camera->near, camera->far);

	glViewport(0, 0, newWidth, newHeight);
}

void OpenGLRenderer::prerender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_CW);

	this->sortedRenderStateKeys.clear();
	for (unsigned int instanceID : scene->instanceCache) {
		RenderStateKey key = 0L;
		//MaterialID matID = MaterialID::NONE_MATERIAL_ID;

		Scene::Instance *instance = &scene->instanceCache[instanceID];
		Scene::Mesh *mesh = &scene->meshCache[instance->meshID];
		MaterialID matID = mesh->typeID;

		RenderState state;
		state.instanceID = instanceID;
		state.materialID = matID;	//todo figure out weird memory access error crash

		key = this->createRenderStateKey(state);
		sortedRenderStateKeys.push_back(key);
	}
	std::sort(this->sortedRenderStateKeys.begin(), this->sortedRenderStateKeys.end());
}

void OpenGLRenderer::render(Voxel::ChunkManager *manager) {
	Scene::Camera *camera = &scene->cameraCache[scene->getMainCameraID()];
	glm::mat4 view = glm::lookAt(camera->eye, camera->eye + camera->target, camera->up);
	glm::mat4 vp = perspectiveProjection * view;
	glm::vec3 camera_position = camera->eye;

	int index = 0;
	index = renderBasic(index, vp);
	index = renderNormal(index, vp);
	index = renderBasicLit(index, camera_position, vp);
	//index = renderBasicBlock(index, camera_position, vp);
	renderChunks(manager, camera_position, vp);
	//glBindVertexArray(0);

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
		RenderState state = this->getRenderStateFromIndex(index);

		Scene::Instance *instance = &scene->instanceCache[state.instanceID];
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
		RenderState state = this->getRenderStateFromIndex(index);

		Scene::Instance *instance = &scene->instanceCache[state.instanceID];
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
		RenderState state = this->getRenderStateFromIndex(index);

		Scene::Instance *instance = &scene->instanceCache[state.instanceID];
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
		RenderState state = this->getRenderStateFromIndex(index);

		Scene::Instance *instance = &scene->instanceCache[state.instanceID];
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

void OpenGLRenderer::renderChunks(Voxel::ChunkManager *manager, glm::vec3 camera_position, glm::mat4 vp) {
	Util::Performance::Timer chunks("RenderChunks");

	const Graphics::BlockMaterial chunkMat = { {.95f, .7f, .8f}, 30 };

	


	Shader::GLSLProgram *shader = Shader::getShaderSet({ "new_chunk_shader.vert", "new_chunk_shader.frag" });
	shader->use();

	//shader->setUniform3f("fog.color", glm::vec3(.15, .15, .15));
	//shader->setUniform1f("fog.start_dist", 250);
	//shader->setUniform1f("fog.attenuation", 0.009);

	shader->setUniform3f("dirLight.dir", glm::vec3(1, -1, 3));
	shader->setUniform3f("dirLight.color", glm::vec3(1, 1, 1));

	shader->setUniform3f("ambient.color", glm::vec3(1, 1, 1));
	shader->setUniform1f("ambient.intensity", .2f);

	shader->setUniform3f("camera_pos", camera_position);
	shader->setUniformMat4("VP", vp);

	Scene::PointLight &point = this->scene->pointLightCache[0];

	this->chunkTex.bindActiveTexture(0);
	for (auto iterator = manager->getVisibleChunks().begin(); iterator != manager->getVisibleChunks().end(); iterator++) {

		auto data = *iterator;

		if (data == nullptr) {
			continue;
		}

		float x = data->getChunkX() * Voxel::CHUNK_RENDER_WIDTH_X;
		float y = data->getChunkY() * Voxel::CHUNK_RENDER_WIDTH_Y;
		float z = data->getChunkZ() * Voxel::CHUNK_RENDER_WIDTH_Z;

		shader->setUniform3f("chunk_pos", glm::vec3(x, y, z));

		data->vao.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glEnableVertexAttribArray(COLOR_ATTRIB_LOCATION);
		glEnableVertexAttribArray(BLOCK_TEXCOORD_ATTRIB_LOCATION);
 		glDrawElements(GL_TRIANGLES, (GLsizei)data->indexCount, GL_UNSIGNED_INT, 0);
	}

	shader->end();
	glBindVertexArray(0);
}

bool OpenGLRenderer::isValidState(int sortedStateKeyIndex, MaterialID typeID) {
	return sortedStateKeyIndex < sortedRenderStateKeys.size()
		&& typeID == getRenderStateFromKey(sortedRenderStateKeys[sortedStateKeyIndex]).materialID;
}

OpenGLRenderer::RenderStateKey OpenGLRenderer::createRenderStateKey(OpenGLRenderer::RenderState state) {
	RenderStateKey key = 0L;

	key |= state.instanceID;
	key |= ((unsigned long long)state.materialID) << sizeof(state.instanceID) * 8;

	return key;
}

OpenGLRenderer::RenderState OpenGLRenderer::getRenderStateFromKey(RenderStateKey key) {
	RenderState state;

	state.instanceID = key & 0xffffffff;
	state.materialID = (MaterialID)((key >> sizeof(state.instanceID) * 8) & 0xff);

	return state;
}

OpenGLRenderer::RenderState OpenGLRenderer::getRenderStateFromIndex(int sortedRenderStateKeyIndex) {
	return getRenderStateFromKey(sortedRenderStateKeys[sortedRenderStateKeyIndex]);
}