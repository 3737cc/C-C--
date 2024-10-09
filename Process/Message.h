#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

// 消息结构体
struct Message {
	int id;          // 消息ID
	std::string body; // 消息内容

	Message(int id_, const std::string& body_) : id(id_), body(body_) {}
};

// 消息队列类
class MessageQueue {
public:
	// 构造函数与析构函数
	MessageQueue() = default;
	~MessageQueue() = default;

	// 添加消息到队列
	void push(const Message& msg);

	// 从队列中获取消息
	Message pop();

	// 判断队列是否为空
	bool empty() const;

private:
	std::queue<Message> queue_;               // 存储消息的队列
	mutable std::mutex mutex_;                // 保护队列的互斥锁
	std::condition_variable cond_var_;        // 用于通知线程消息到达的条件变量
};

#endif // MESSAGE_H
