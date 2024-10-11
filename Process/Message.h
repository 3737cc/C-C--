#ifndef MESSAGE_H
#define MESSAGE_H

#include <windows.h>
#include <mq.h>
#include <string>
#include <vector>
#include <queue>

// 需要链接 mqrt.lib
#pragma comment(lib, "mqrt.lib")

// 消息结构体
struct Message {
	int id;
	std::string body;

	Message(int id = 0, const std::string& body = "") : id(id), body(body) {}
};

class MessageQueue {
public:
	MessageQueue(const std::string& queueName);
	~MessageQueue();

	bool createQueue();
	void printErrorDetails(HRESULT hr);
	bool openQueue(bool isReceive);
	bool send(const Message& msg);
	bool receive(Message& msg);
	bool empty()const;
	size_t size()const;
	void close();

private:
	std::queue<Message> m_messages;
	std::string m_szQueueName;
	std::string m_szFormatName;
	QUEUEHANDLE m_queueHandle;

	static std::string getFormatName(const std::string& queueName);
};

#endif // MESSAGE_H