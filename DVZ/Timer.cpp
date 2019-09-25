#include "Timer.h"
#include "macrologger.h"
#include <stdio.h>

using namespace Util::Performance;

std::string Util::Performance::getTimerInfo() {
	printf("--------------------------------------------------------------------------------------------------------\n");
	auto iter = Internal::timings.begin();
	while (iter != Internal::timings.end()) {
		std::vector<long long> &durrations = iter->second;

		if (durrations.size() == 0) {
			printf("%s:\t\t [mean]: 0.00 | [min]: 0.00 | [max]: 0.00 | [total]: 0.00 | [samples]: 0\n", iter->first.c_str());
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

		printf("%s:\t\t[mean]: %.2lf | [min]: %.2lf | [max]: %.2lf | [total]: %.2lf | [samples]: %d\n", 
			iter->first.c_str(), mean / 1000.0f, min / 1000.0f, max / 1000.0f, total / 1000.0f, count);

		durrations.clear();

		iter++;
	}


	printf("--------------------------------------------------------------------------------------------------------\n\n");
	return "";
}

Timer::Timer(std::string message){
	this->message = message;
	this->start = std::chrono::high_resolution_clock::now();
}



Timer::~Timer() {
	auto end = std::chrono::high_resolution_clock::now();
	auto durration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	Internal::timings[this->message].push_back(durration);
}

