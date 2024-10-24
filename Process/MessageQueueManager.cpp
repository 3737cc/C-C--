#include "MessageQueueManager.h"

MessageQueueManager::MessageQueueManager()
	: m_messageQueue(nullptr)
	, m_maxMessageSize(0)
	, m_maxMessages(0)
	, m_bInitialized(false)
	, m_chMessageQueue("messageQueue")
{
}

MessageQueueManager::~MessageQueueManager()
{
	if (m_messageQueue) {
		message_queue::remove(m_chMessageQueue);
		delete m_messageQueue;
		m_messageQueue = nullptr;
	}
}

bool MessageQueueManager::Connect(const char* queueName, size_t maxMessages, size_t maxMessageSize)
{
	m_chMessageQueue = queueName;
	if (m_bInitialized) {
		return false;
	}

	try {
		m_messageQueue = new message_queue(open_or_create, m_chMessageQueue, maxMessages, maxMessageSize);
		m_maxMessageSize = maxMessageSize;
		m_maxMessages = maxMessages;
		m_bInitialized = true;
		return true;
	}
	catch (const interprocess_exception&) {
		m_messageQueue = nullptr;
		return false;
	}
}

bool MessageQueueManager::Disconnect() {
	if (m_messageQueue) {
		// 先移除消息队列
		message_queue::remove(m_chMessageQueue);

		// 然后删除消息队列对象
		delete m_messageQueue;
		m_messageQueue = nullptr; // 将指针设为 nullptr，避免悬空指针
	}
	m_bInitialized = false;
	return true; // 返回值可以根据需要调整
}

bool MessageQueueManager::SendData(const std::string& message)
{
	if (!m_bInitialized || !m_messageQueue) {
		return false;
	}

	if (message.size() > m_maxMessageSize) {
		return false;
	}

	try {
		m_messageQueue->send(message.c_str(), message.size(), 0);
		return true;
	}
	catch (const interprocess_exception&) {
		return false;
	}
}

bool MessageQueueManager::receiveMessage(std::vector<char>& buffer, size_t& receivedSize)
{
	if (!m_bInitialized || !m_messageQueue) {
		return false;
	}

	buffer.resize(m_maxMessageSize);
	unsigned int priority;

	try {
		return m_messageQueue->try_receive(buffer.data(), buffer.size(), receivedSize, priority);
	}
	catch (const interprocess_exception&) {
		return false;
	}
}

unsigned int MessageQueueManager::getMessageCount() const
{
	return m_bInitialized ? m_messageQueue->get_num_msg() : 0;
}

size_t MessageQueueManager::getMaxMessageSize() const
{
	return m_maxMessageSize;
}

bool MessageQueueManager::isInitialized() const
{
	return m_bInitialized;
}

size_t MessageQueueManager::getMaxMessages() const
{
	return m_maxMessages;
}
