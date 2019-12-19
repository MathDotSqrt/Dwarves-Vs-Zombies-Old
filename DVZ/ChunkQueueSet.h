#pragma once

#include "ChunkRefHandle.h"
#include <unordered_set>
#include <deque>

namespace Voxel {
	class ChunkQueueSet {
	public:

		bool can_push(int hashcode) {
			return m_lookup.find(hashcode) == m_lookup.end();
		}


		void push(ChunkRefHandle &&handle) {
			int hashcode = handle->getHashCode();
			if (can_push(hashcode)) {
				m_queue.push_back(std::move(handle));
				m_lookup.insert(hashcode);
			}
		}

		void pop() {
			m_lookup.erase(m_queue.back()->getHashCode());
			m_queue.pop_front();
		}

		ChunkRefHandle& front() {
			return m_queue.front();
		}
		
		const ChunkRefHandle& front() const {
			return m_queue.front();
		}

		size_t size() const {
			return m_queue.size();
		}

		bool empty() const {
			return m_queue.empty();
		}

	private:
		std::unordered_set<int> m_lookup;
		std::deque<ChunkRefHandle> m_queue;
	};
}