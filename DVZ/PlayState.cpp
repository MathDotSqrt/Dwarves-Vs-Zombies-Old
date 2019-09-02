//#define _HAS_STD_BYTE 0	//Visual Studio preprocessor value to turn off conflicting std::byte and byte for c++ 17
#include "PlayState.h"
#include "macrologger.h"
#include "QuadGeometry.h"
#include "CubeGeometry.h"
#include "ModelGeometry.h"
#include "Components.h"
#include "MovementSystem.h"
#include "InputSystem.h"
#include "BasicRenderSystem.h"
#include "NetPlayerSystem.h"
#include "VoxelSystem.h"
#include "ChunkManager.h"
#include "Attrib.h"

PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {

}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_STATE("init");

	/*PLAYER*/
	entt::entity playerID = this->e.addPlayer(1, 0, 0);
	unsigned int pointLightInstanceID = this->e.getScene()->createPointLightInstance();
	this->e.assign<PointLightComponent>(playerID, pointLightInstanceID, glm::vec3(1, 1, 1), 60.0f);
	/*PLAYER*/
	Graphics::VAO vao;
	Graphics::VBO vbo(GL_VERTEX_ARRAY);

	//vao.bufferInterleavedData(vbo, Graphics::Attrib<POSITION_ATTRIB_LOCATION, glm::vec3>(), Graphics::Attrib<NORMAL_ATTRIB_LOCATION, glm::vec3>());
	Graphics::Attrib<POSITION_ATTRIB_LOCATION, glm::vec3> a;

	LOG_ERROR("ATTRIB %d", a.getNumComponents());
	LOG_ERROR("ATTRIB %zu", a.getSizeOfAttrib());
	LOG_ERROR("ATTRIB %d", a.getAttribOption());
	LOG_ERROR("ATTRIB %d", glm::vec3::length);
	
	/*FLOOR*/
	Graphics::QuadGeometry quad;
	Graphics::BasicLitMaterial c = { {1, 1, 1}, {.5f, .7f, .1f}, 10 };
	unsigned int meshID = this->e.getScene()->createMesh(quad, c);
	Graphics::Transformation t = { glm::vec3(0, -1, 0), glm::vec3(3.1415f / 2, 0, 0), glm::vec3(10, 10, 10) };
	unsigned int renderID = this->e.getScene()->createRenderInstance(meshID, t);
	/*FLOOR*/

	/*DRAGONS*/
	Graphics::ModelGeometry dragon("tree.obj");
	for (int i = 0; i < 100; i++) {
		entt::entity obj = this->e.create();
		this->e.assign<PositionComponent>(obj, glm::vec3((i - 50) * 5, -1, -10));
		this->e.assign<RotationComponent>(obj, glm::quat(glm::vec3(0, 0, 0)));
		this->e.assign<ScaleComponent>(obj, glm::vec3(.3f, .3f, .3f));
		this->e.assign<RotationalVelocityComponent>(obj, glm::vec3(0, 1, 0));
		unsigned int meshID2;
		if (i % 3 == 0) {
			Graphics::ColorMaterial cm = {i / 100.0f, 1 - i / 100.0f, .5f};
			meshID2 = this->e.getScene()->createMesh(dragon, cm);
		}
		else if (i % 3 == 1) {
			Graphics::NormalMaterial nm;
			meshID2 = this->e.getScene()->createMesh(dragon, nm);
		}
		else {
			Graphics::BasicLitMaterial bm = { {1, 1, 1}, {1, 1, 1}, 100};
			meshID2 = this->e.getScene()->createMesh(dragon, bm);
		}
		
		renderID = this->e.getScene()->createRenderInstance(meshID2);
		this->e.assign<RenderInstanceComponent>(obj, renderID);
	}
	/*DRAGONS*/

	/*CHUNKS*/
	//Voxel::ChunkManager *manager = this->e.getChunkManager();

	//Voxel::Chunk *chunk = manager->getChunk(0, 0, 0);
	//Voxel::Block grass(Voxel::BlockType::BLOCK_TYPE_GRASS);
	//manager->setBlock(0.0f, 0.0f, 1.0f, grass);
	//manager->setBlock(1.0f, 0.0f, 1.0f, grass);
	//manager->setBlock(2.0f, 0.0f, 1.0f, grass);
	//manager->setBlock(3.0f, 0.0f, 1.0f, grass);
	//chunk->generateMesh();
	/*CHUNKS*/

	/*NET*/
	//e.attemptConnection("54.224.40.47", 60000);	//AWS
	e.attemptConnection("127.0.0.1", 60000);		//LOCAL
	/*NET*/

	/*SYSTEM*/
	this->e.addSystem(new InputSystem(0));
	this->e.addSystem(new MovementSystem(1));
	this->e.addSystem(new NetPlayerSystem(.1f, 2));
	this->e.addSystem(new VoxelSystem(3));
	this->e.addSystem(new BasicRenderSystem(100));
	/*SYSTEM*/

}

void PlayState::cleanUp() {
	LOG_STATE("cleanUp");
}

void PlayState::entered() {
	LOG_STATE("entered");
}

void PlayState::leaving() {
	LOG_STATE("leaving");

}

void PlayState::update(float delta) {
	this->e.update(delta);
}