//#pragma once
//
//
//namespace Util::Threading{
//
//
//template<typename T>
//class IThreadPoolTask {
//public:
//	typedef std::unique_ptr<IThreadPoolTask<T>> ThreadPoolTaskPtr;
//	// ThreadPoolTaskPtr = std::unique_ptr<IThreadPoolTask<T>>;
//
//	IThreadPoolTask() : isDone(false) {}
//	~IThreadPoolTask() {}
//
//	virtual void execute(T *data) = 0;
//	virtual void cleanUp() {};
//
//	void finished() {
//		this->isDone = true;
//	}
//private:
//	volatile bool isDone = false;
//};
//
//}