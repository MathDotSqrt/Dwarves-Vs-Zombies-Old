//#pragma once
//
//#include <unordered_set>
//#include <deque>
//
//namespace std {
//
//	// partial specialization for reference_wrapper
//	// is this really necessary?
//	template<typename T>
//	class hash<std::reference_wrapper<T>> {
//	public:
//		std::size_t operator()(std::reference_wrapper<T> x) const
//		{
//			return std::hash<T>()(x.get());
//		}
//	};
//
//}
//
//namespace Util {
//	
//
//	template <typename T>
//	class SetQueue {
//		// important: this really needs to be a deque and only front
//		// insertion/deletion is allowed to not get dangling references
//		typedef std::deque<T> storage;
//		typedef std::reference_wrapper<const T> c_ref_w;
//	public:
//		typedef typename storage::value_type value_type;
//		typedef typename storage::reference reference;
//		typedef typename storage::const_reference const_reference;
//		typedef typename storage::size_type size_type;
//
//		// no move semantics
//		void push(const T& t) {
//			auto it = lookup_.find(std::cref(t));
//			if (it == end(lookup_)) {
//				store_.push_back(t);
//				lookup_.insert(store_.back());
//			}
//		}
//
//		void pop() {
//			lookup_.erase(store_.front()); store_.pop_front();
//		}
//
//		const T& front() const { 
//			return store_.front(); 
//		}
//		T& front() { 
//			return store_.front(); 
//		}
//
//		bool empty() const {
//			return store_.empty();
//		}
//
//		bool size() const {
//			return store_.size();
//		}
//		
//	private:
//		// look-up mechanism
//		std::unordered_set<c_ref_w> lookup_;
//		// underlying storage
//		storage store_;
//	};
//
//} 