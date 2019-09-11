#include "ShaderUpdaterSystem.h"
#include "macrologger.h"

#include "ShaderSet.h"
#include "windows.h"
#include "fileapi.h"


ShaderUpdaterSystem::ShaderUpdaterSystem(float interval, int priority) : IntervalSystem(interval, priority) {

}


ShaderUpdaterSystem::~ShaderUpdaterSystem() {
}

void ShaderUpdaterSystem::addedToEngine(Engine *engine) {
	LOG_SYSTEM("added to engine");
}

void ShaderUpdaterSystem::removedFromEngine(Engine *engine) {
	LOG_SYSTEM("removed from engine");
}

void ShaderUpdaterSystem::updateInterval(Engine *engine, float delta) {
	Graphics::Shader::ShaderIterator iterator = Graphics::Shader::begin();

	while (iterator != Graphics::Shader::end()) {
		Graphics::Shader::GLSLProgram *program = iterator->second;
		bool shaderNeedsUpdate = false;

		for (auto& fileData : program->getFiledata()) {
			HANDLE fileHandle = (HANDLE)fileData.first;
			FILETIME lastWriteTime = { fileData.second.low, fileData.second.high };
			if (fileHandle != INVALID_HANDLE_VALUE) {
				FILETIME currentWriteTime = {0};
				GetFileTime(fileHandle, NULL, NULL, &currentWriteTime);

				bool fileUpdated = false;

				if (currentWriteTime.dwHighDateTime > lastWriteTime.dwHighDateTime) {
					fileUpdated |= true;
				}
				else if (currentWriteTime.dwHighDateTime == lastWriteTime.dwHighDateTime
					&& currentWriteTime.dwLowDateTime > lastWriteTime.dwLowDateTime) {
					fileUpdated |= true;
				}

				if (fileUpdated) {
					fileData.second.low = currentWriteTime.dwLowDateTime;
					fileData.second.high = currentWriteTime.dwHighDateTime;
					shaderNeedsUpdate = true;
				}
				
			}
		}
		if (shaderNeedsUpdate) {
			LOG_SYSTEM("Updating Shader...");
			Graphics::Shader::reloadShader(&program);
		}

		iterator++;
	}
}
