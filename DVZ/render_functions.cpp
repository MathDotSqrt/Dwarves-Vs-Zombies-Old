#include "render_functions.h"
#include "Scene.h"
#include "ChunkRenderDataManager.h"
#include "ShaderSet.h"
#include "Timer.h"
#include "TEX.h"
#include "DepthFBO.h"
#include "preamble.h"


using namespace Graphics;
using namespace ShaderVariables;

namespace Graphics::ShaderVariables {
	extern float shader_time = 0;
	extern glm::vec3 camera_pos = glm::vec3(0);
	extern glm::mat4 p = glm::mat4(1);
	extern glm::mat4 v = glm::mat4(1);
	extern glm::mat4 vp = glm::mat4(1);
	extern glm::mat4 sunVP = glm::mat4(1);
}

inline bool is_valid(iterator start, iterator end, MaterialID id) {
	return start != end && start->getMaterialID() == id;
}

iterator Graphics::render_basic(Scene *scene, iterator start, iterator end) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Shader::GLSLProgram *shader = Shader::getShaderSet({ "res/basic_shader.vert", "res/basic_shader.frag" });
	shader->use();
	do {
		RenderStateKey key = *start;
		Scene::Instance &instance = scene->instanceCache[key.getValue()];

		Scene::Transformation &transformation = scene->transformationCache[instance.transformationID];
		ColorMaterial &material = scene->colorMaterialCache[instance.materialID];
		const auto &mesh = instance.meshHandle;


		glm::quat rot(transformation.rotation);
		glm::vec3 axis = glm::axis(rot);

		float angle = glm::angle(rot);

		glm::mat4 model;
		model = glm::translate(identity, transformation.position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, transformation.scale);

		shader->setUniformMat4("MVP", vp * model);
		shader->setUniform3f("color", material.color);

		mesh->vao.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);

		start++;
	} while (is_valid(start, end, MaterialID::COLOR_MATERIAL_ID));

	shader->end();
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return start;
}

iterator Graphics::render_normal(Scene *scene, iterator start, iterator end) { 
	glm::mat4 ident = glm::identity<glm::mat4>();
	Shader::GLSLProgram *shader = Shader::getShaderSet({ "res/normal_shader.vert", "res/normal_shader.frag" });
	shader->use();

	do {
		RenderStateKey state = *start;

		Scene::Instance &instance = scene->instanceCache[state.getValue()];
		Scene::Transformation &transformation = scene->transformationCache[instance.transformationID];
		const auto &mesh = instance.meshHandle;

		glm::quat rot(transformation.rotation);
		glm::vec3 axis = glm::axis(rot);

		float angle = glm::angle(rot);

		glm::mat4 model;
		model = glm::translate(ident, transformation.position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, transformation.scale);

		glm::mat3 mat(model);
		shader->setUniformMat3("inverseTransposeMatrix", glm::inverse(mat), true);
		shader->setUniformMat4("MVP", vp * model);

		mesh->vao.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);

		start++;
	} while (is_valid(start, end, MaterialID::NORMAL_MAERIAL_ID));

	shader->end();
	glBindVertexArray(0);

	return start;
}

iterator Graphics::render_basic_lit(Scene *scene, iterator start, iterator end) {
	Shader::GLSLProgram *shader = Shader::getShaderSet({ "res/basic_lit_shader.vert", "res/basic_lit_shader.frag" });
	shader->use();

	shader->setUniform3f("camera_pos", camera_pos);
	shader->setUniformMat4("VP", vp);

	Scene::PointLight &point = scene->pointLightCache[0];
	shader->setUniform3f("point_light_position", point.position);
	shader->setUniform3f("point_light_color", point.color);
	shader->setUniform1f("point_light_intensity", point.intensity);

	do {
		RenderStateKey state = *start;

		Scene::Instance &instance = scene->instanceCache[state.getValue()];
		Scene::Transformation &transformation = scene->transformationCache[instance.transformationID];
		BasicLitMaterial &material = scene->basicLitMaterialCache[instance.materialID];
		const auto &mesh = instance.meshHandle;

		glm::quat rot(transformation.rotation);
		glm::vec3 axis = glm::axis(rot);

		float angle = glm::angle(rot);

		glm::mat4 model;
		model = glm::translate(identity, transformation.position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, transformation.scale);

		glm::mat3 mat(model);
		shader->setUniformMat3("inverseTransposeMatrix", glm::inverse(mat), true);
		shader->setUniformMat4("M", model);

		shader->setUniform3f("diffuse_color", material.diffuseColor);
		shader->setUniform3f("specular_color", material.specularColor);
		shader->setUniform1f("shinyness", material.shinyness);

		mesh->vao.bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);

		start++;
	} while (is_valid(start, end, MaterialID::BASIC_LIT_MATERIAL_ID));

	shader->end();
	glBindVertexArray(0);

	return start;
}

iterator Graphics::render_chunks(Voxel::ChunkRenderDataManager *manager, Scene *scene, DepthFBO *fbo, iterator start, iterator end) {
	Util::Performance::Timer chunks("RenderChunks");

	constexpr glm::vec3 CHUNK_SCALE(Voxel::CHUNK_RENDER_WIDTH_X, Voxel::CHUNK_RENDER_WIDTH_Y, Voxel::CHUNK_RENDER_WIDTH_Z);

	Shader::GLSLProgram *shader = Shader::getShaderSet({ "res/new_chunk_shader.vert", "res/new_chunk_shader.frag" });
	shader->use();

	shader->setUniform3f("fog.color", glm::vec3(.13, .13, .13));
	shader->setUniform1f("fog.start_dist", 0);
	shader->setUniform1f("fog.attenuation", 0.004f);

	auto dir = scene->cameraCache[scene->getSunCameraID()].target;
	//shader->setUniform3f("dirLight.dir", glm::vec3(1, -1, 3));
	shader->setUniform3f("dirLight.dir", dir);
	shader->setUniform3f("dirLight.color", glm::vec3(1, 1, 1));

	shader->setUniform3f("ambient.color", glm::vec3(1, 1, 1));
	shader->setUniform1f("ambient.intensity", .2f);

	shader->setUniform3f("camera_pos", camera_pos);
	shader->setUniformMat4("VP", vp);
	shader->setUniformMat4("sun_VP", ShaderVariables::sunVP);

	Scene::PointLight &point = scene->pointLightCache[0];

	//shader->setUniform1i("debug", 0);
	shader->setUniform1i("tex", 0);
	shader->setUniform1i("shadowTex", 1);
	//manager->debugTEX.bindActiveTexture(0);
	manager->worldTEX.bindActiveTexture(0);
	fbo->getDepthAttachment().bindActiveTexture(1);


	for (auto iterator = manager->begin(); iterator != manager->end(); iterator++) {

		auto data = *iterator;

		shader->setUniform3f("chunk_pos", data.pos * CHUNK_SCALE);
		shader->setUniform1f("time", shader_time - data.startTime);

		glBindVertexArray(data.vaoID);
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glEnableVertexAttribArray(NORMAL_ATTRIB_LOCATION);
		glEnableVertexAttribArray(BLOCK_TEXCOORD_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, (GLsizei)data.indexCount, GL_UNSIGNED_INT, 0);
	}

	shader->end();
	glBindVertexArray(0);

	return ++start;
}

iterator Graphics::render_chunks_shadow(Voxel::ChunkRenderDataManager *manager, Scene *scene, iterator start, iterator end) {
	const glm::vec3 CHUNK_SCALE(Voxel::CHUNK_RENDER_WIDTH_X, Voxel::CHUNK_RENDER_WIDTH_Y, Voxel::CHUNK_RENDER_WIDTH_Z);
	Shader::GLSLProgram *shader = Shader::getShaderSet({ "res/chunk_shadow_shader.vert", "res/chunk_shadow_shader.frag" });
	shader->use();

	shader->setUniformMat4("VP", vp);

	for (auto iterator = manager->begin(); iterator != manager->end(); iterator++) {

		auto data = *iterator;
		
		glm::mat4 M = glm::identity<glm::mat4>();
		M = glm::translate(M, data.pos * CHUNK_SCALE);

		glm::mat4 MVP = ShaderVariables::vp * M;
		
		glm::vec3 render_pos = data.pos * CHUNK_SCALE + CHUNK_SCALE * .5f;
		if (render_pos.x > 20 || render_pos.x < -20 || render_pos.z > 20 || render_pos.z < -20)
			continue;

		shader->setUniformMat4("MVP", MVP);
		glBindVertexArray(data.vaoID);
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, (GLsizei)data.indexCount, GL_UNSIGNED_INT, 0);
	}

	shader->end();
	glBindVertexArray(0);

	return ++start;

}
