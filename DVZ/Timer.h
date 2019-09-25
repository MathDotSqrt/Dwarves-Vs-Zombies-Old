#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
namespace Util::Performance {
	
	namespace Internal {
		static std::unordered_map<std::string, std::vector<long long>> timings;
	}

	std::string getTimerInfo();

	class Timer {
	public:
		Timer(std::string message);
		~Timer();

	private:
		std::string message;
		std::chrono::steady_clock::time_point start;
	};
}

