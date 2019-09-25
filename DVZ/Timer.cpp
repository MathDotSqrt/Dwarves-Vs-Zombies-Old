#include "Timer.h"
#include "macrologger.h"
#include <stdio.h>

using namespace Util::Performance;

std::string Util::Performance::getTimerInfo() {
	printf("\n--------------------------------------------------------------------------------------\n");
	auto iter = Internal::timings.begin();
	while (iter != Internal::timings.end()) {
		std::vector<long long> &durrations = iter->second;

		if (durrations.size() == 0) {
			printf("%s:\t\t[min]: 0 | [mean]: 0.000000 | [max]: 0 | [total]: 0 | [samples]: 0\n", iter->first.c_str());
			iter++;
			continue;
		};

		auto durrIter = durrations.begin();
		long long min = *durrIter;
		long long max = 0;
		long long total = 0;

		int count = 0;
		double mean = 0;

		while (durrIter != durrations.end()) {
			long long durration = *durrIter;

			if (durration < min) {
				min = durration;
			}
			if (durration > max) {
				max = durration;
			}

			total += durration;
			count++;
			durrIter++;
		}

		mean = (double)total / count;

		printf("%s:\t\t[min]: %lld | [mean]: %lf | [max]: %lld | [total]: %lld | [samples]: %d\n", iter->first.c_str(), min, mean, max, total, count);
		durrations.clear();

		iter++;
	}


	printf("--------------------------------------------------------------------------------------\n");
	return "";
}

Timer::Timer(std::string message){
	this->message = message;
	this->start = std::chrono::high_resolution_clock::now();
}



Timer::~Timer() {
	auto end = std::chrono::high_resolution_clock::now();
	auto durration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	Internal::timings[this->message].push_back(durration);
}

