#include "Signal.h"

// 构造函数，初始化信号量的计数值
Semaphore::Semaphore(int count) : m_iCount(count) {}

// Wait 函数：当计数器为 0 时，阻塞线程，等待信号量
void Semaphore::Wait() {
	std::unique_lock<std::mutex> lock(mtx); // 获取锁
	// 如果计数器为 0，则等待
	cv.wait(lock, [this]() { return m_iCount > 0; });
	// 当计数器大于 0 时，减少计数器
	--m_iCount;
}

// Signal 函数：释放信号量，增加计数器，并通知等待的线程
void Semaphore::Signal() {
	std::unique_lock<std::mutex> lock(mtx); // 获取锁
	++m_iCount;                                // 增加计数器
	cv.notify_one();                        // 唤醒一个等待的线程
}
