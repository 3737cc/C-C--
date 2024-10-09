#include "message.h"

// 向队列中添加消息
void MessageQueue::push(const Message& msg) {
	std::lock_guard<std::mutex> lock(mutex_);
	queue_.push(msg);
	cond_var_.notify_one();  // 通知等待的线程有新的消息
}

// 从队列中获取消息
Message MessageQueue::pop() {
	std::unique_lock<std::mutex> lock(mutex_);
	// 等待直到队列中有消息
	cond_var_.wait(lock, [this]() { return !queue_.empty(); });

	// 获取队列中的消息
	Message msg = queue_.front();
	queue_.pop();
	return msg;
}

// 判断队列是否为空
bool MessageQueue::empty() const {
	std::lock_guard<std::mutex> lock(mutex_);
	return queue_.empty();
}
