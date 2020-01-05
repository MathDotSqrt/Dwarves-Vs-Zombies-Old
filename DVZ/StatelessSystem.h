#pragma once
#include <chrono>

class Engine;

class StatelessSystem {
private:
	typedef void(*function_ptr)(Engine &engine, float delta);
	typedef std::chrono::duration<float> FrameTime;

	function_ptr ptr;
	FrameTime intervalTime;
	FrameTime currentTimeDuration;
public:
	StatelessSystem(function_ptr ptr, FrameTime interval = FrameTime(0));
	
	bool operator==(const StatelessSystem &other);
	void update(Engine &engine, float delta);

};

