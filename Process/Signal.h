#ifndef MESSAGE_H
#define MESSAGE_H

#include <mutex>
#include <condition_variable>

class Semaphore {
public:
	explicit Semaphore(int count = 0);
	void Wait();    // �ȴ��ź���
	void Signal();  // �ͷ��ź���

private:
	std::mutex mtx;                   // ������
	std::condition_variable cv;       // ��������
	int m_iCount;                        // ������
};

#endif // MESSAGE_H
