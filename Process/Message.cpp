#include "message.h"

// ������������Ϣ
void MessageQueue::push(const Message& msg) {
	std::lock_guard<std::mutex> lock(mutex_);
	queue_.push(msg);
	cond_var_.notify_one();  // ֪ͨ�ȴ����߳����µ���Ϣ
}

// �Ӷ����л�ȡ��Ϣ
Message MessageQueue::pop() {
	std::unique_lock<std::mutex> lock(mutex_);
	// �ȴ�ֱ������������Ϣ
	cond_var_.wait(lock, [this]() { return !queue_.empty(); });

	// ��ȡ�����е���Ϣ
	Message msg = queue_.front();
	queue_.pop();
	return msg;
}

// �ж϶����Ƿ�Ϊ��
bool MessageQueue::empty() const {
	std::lock_guard<std::mutex> lock(mutex_);
	return queue_.empty();
}
