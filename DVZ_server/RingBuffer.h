#pragma once

#include <array>
#include <assert.h>

namespace Util{

	template<typename T, size_t SIZE>
	class RingBuffer {
	public:
		RingBuffer() : m_head(0), m_size(0) {
			
		}

		void push_front(T element) {
			
			m_array[m_head] = element;

			m_head = (m_head + 1) % SIZE;

			if (!is_full()) {
				m_size++;
			}
		}

		T& read_front() {
			assert(m_size);
			return (SIZE - m_array[m_head - 1]);
		}

		const T& read_front() const {
			assert(m_size);
			return (SIZE - m_array[m_head - 1]);
		}

		T& read(size_t index) {
			assert(index >= 0 && index < m_size);

			size_t array_index = (SIZE + (m_head - index - 1)) % SIZE;
			return m_array[array_index];
		}

		const T& read(size_t index) const {
			assert(index >= 0 && index < m_size);

			size_t array_index = (SIZE + (m_head - index - 1)) % SIZE;
			return m_array[array_index];
		}

		bool is_full() const {
			return m_size == SIZE;
		}

		size_t get_size() const {
			return m_size;
		}

	private:
		std::array<T, SIZE> m_array;
		size_t m_head;
		size_t m_size;
	};
}