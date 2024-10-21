#ifndef MESSAGE_QUEUE_MANAGER_H
#define MESSAGE_QUEUE_MANAGER_H

#ifdef MESSAGEQUEUEAPI
#define MESSAGEQUEUEAPI __declspec(dllexport)
#else
#define MESSAGEQUEUEAPI __declspec(dllimport)
#endif

#include <boost/interprocess/ipc/message_queue.hpp>
#include <string>
#include <vector>

using namespace boost::interprocess;

class MESSAGEQUEUEAPI MessageQueueManager {
public:
	MessageQueueManager();
	virtual ~MessageQueueManager();

	// 初始化消息队列
	bool initialize(const char* queueName, size_t maxMessages, size_t maxMessageSize);

	// 发送消息
	bool sendMessage(const std::string& message);

	// 读取消息
	bool receiveMessage(std::vector<char>& buffer, size_t& receivedSize);

	// 获取消息队列中的消息数量
	unsigned int getMessageCount() const;

	// 获取最大消息大小
	size_t getMaxMessageSize() const;

	// 检查消息队列是否已初始化
	bool isInitialized() const;

	// 获取最大消息数量
	size_t getMaxMessages() const;

	//释放资源
	void remove();

private:
	message_queue* m_messageQueue;
	size_t m_maxMessageSize;
	size_t m_maxMessages;
	bool m_initialized;
};

#endif // MESSAGE_QUEUE_MANAGER_H
