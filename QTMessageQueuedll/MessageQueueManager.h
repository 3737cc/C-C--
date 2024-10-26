// MessageQueueManager.h
#ifndef MESSAGE_QUEUE_MANAGER_H
#define MESSAGE_QUEUE_MANAGER_H

#include <boost/interprocess/ipc/message_queue.hpp>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <QDateTime>
#include<QTimer>
#include <QObject> 

#if defined(MESSAGEQUEUEAPI_EXPORTS)
#define MESSAGEQUEUE_API __declspec(dllexport)
#else
#define MESSAGEQUEUE_API __declspec(dllimport)
#endif

// 声明元对象数据
#if defined(MESSAGEQUEUEAPI_EXPORTS)
class MESSAGEQUEUE_API MessageQueueManager;
#else
class MessageQueueManager;
#endif

using namespace boost::interprocess;

class MESSAGEQUEUE_API MessageQueueManager : public QObject {
	Q_OBJECT

public:
	explicit MessageQueueManager(QObject* parent = nullptr);
	virtual ~MessageQueueManager();

	// 创建消息队列
	bool Connect(const char* queueName, size_t maxMessages, size_t maxMessageSize);
	// 释放资源
	bool Disconnect();
	// 发送消息
	bool sendData(const std::string& message);
	// 读取消息
	bool receiveMessage(std::vector<char>& buffer, size_t& receivedSize);
	// 启动自动获取
	void startAutoReceive(int interval);
	// 停止自动获取
	void stopAutoReceive();
	// 获取消息队列中的消息数量
	unsigned int getMessageCount() const;
	// 获取最大消息大小
	size_t getMaxMessageSize() const;
	// 检查消息队列是否已初始化
	bool isInitialized() const;
	// 获取最大消息数量
	size_t getMaxMessages() const;

private:
	message_queue* m_messageQueue;
	void autoReceiveMessages();
	bool hasMessages(); // 检查队列是否有消息
	void processMessage(const std::vector<char>& buffer, size_t size);
	size_t m_maxMessageSize;//最大消息大小
	size_t m_maxMessages;//最多发送多少条消息
	bool m_bInitialized;//判断连接的标志位
	const char* m_chMessageQueue;//消息队列的键
	QTimer* m_timer;
	bool m_autoReceiving;//获取是否存在数据
	bool m_hasPrintedEmptyMessage;

signals:
	void messageReceived(const QString& message);
};

#endif // MESSAGE_QUEUE_MANAGER_H