#pragma once

#include <set>
#include <queue>

namespace Util {
	template<typename T>
	class SetQueue {
	public:
		public SetQueue() {
		
		}

		bool push(const T &element) {
			if (set.find(element) == set.end()) {
				queue.push(element);
				set.insert(std::ref(queue.back()));
				return true;
			}

			return false;
		}

		bool push(T &&element) {
			if (set.find(element) == set.end()) {
				queue.emplace(element);
				set.insert(std::ref(queue.back()));
				return true;
			}

			return false;
		}

		void pop() {
			set.erase(std:find(std::ref(queue.front())));
			T element = std::move(queue.front());
			queue.pop();
		}

		T& front() {
			return queue.front();
		}

		const T& front() const {
			return queue.front();
		}

		size_t size() const {
			return queue.size();
		}

		bool empty() const {
			return queue.empty();
		}

	private:
		std::set<std::reference_wrapper<T>> set;
		std::queue<T> queue;
	};
} 