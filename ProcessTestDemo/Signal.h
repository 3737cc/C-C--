#ifndef MESSAGE_H
#define MESSAGE_H

#include <mutex>
#include <condition_variable>

class Semaphore {
public:
	explicit Semaphore(int count = 0);
	void Wait();    // 等待信号量
	void Signal();  // 释放信号量

private:
	std::mutex mtx;                   // 互斥锁
	std::condition_variable cv;       // 条件变量
	int m_iCount;                        // 计数器
};

#endif // MESSAGE_H
