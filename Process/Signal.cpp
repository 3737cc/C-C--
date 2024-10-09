#include "Signal.h"

// ���캯������ʼ���ź����ļ���ֵ
Semaphore::Semaphore(int count) : m_iCount(count) {}

// Wait ��������������Ϊ 0 ʱ�������̣߳��ȴ��ź���
void Semaphore::Wait() {
	std::unique_lock<std::mutex> lock(mtx); // ��ȡ��
	// ���������Ϊ 0����ȴ�
	cv.wait(lock, [this]() { return m_iCount > 0; });
	// ������������ 0 ʱ�����ټ�����
	--m_iCount;
}

// Signal �������ͷ��ź��������Ӽ���������֪ͨ�ȴ����߳�
void Semaphore::Signal() {
	std::unique_lock<std::mutex> lock(mtx); // ��ȡ��
	++m_iCount;                                // ���Ӽ�����
	cv.notify_one();                        // ����һ���ȴ����߳�
}
