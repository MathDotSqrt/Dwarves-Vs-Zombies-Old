#pragma once
#include <chrono>

class EntityAdmin;

class StatelessSystem {
public:
	typedef void(*function_ptr)(EntityAdmin &admin, float delta);
	typedef std::chrono::duration<float> FrameTime;



	StatelessSystem(function_ptr ptr, FrameTime interval = FrameTime(0));
	~StatelessSystem();

	bool operator==(const StatelessSystem &other);
	void update(EntityAdmin &admin, float delta);

private:
	function_ptr ptr;
	FrameTime intervalTime;
	FrameTime currentTimeDuration;

};

