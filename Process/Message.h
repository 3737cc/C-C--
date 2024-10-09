#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

// ��Ϣ�ṹ��
struct Message {
	int id;          // ��ϢID
	std::string body; // ��Ϣ����

	Message(int id_, const std::string& body_) : id(id_), body(body_) {}
};

// ��Ϣ������
class MessageQueue {
public:
	// ���캯������������
	MessageQueue() = default;
	~MessageQueue() = default;

	// �����Ϣ������
	void push(const Message& msg);

	// �Ӷ����л�ȡ��Ϣ
	Message pop();

	// �ж϶����Ƿ�Ϊ��
	bool empty() const;

private:
	std::queue<Message> queue_;               // �洢��Ϣ�Ķ���
	mutable std::mutex mutex_;                // �������еĻ�����
	std::condition_variable cond_var_;        // ����֪ͨ�߳���Ϣ�������������
};

#endif // MESSAGE_H
