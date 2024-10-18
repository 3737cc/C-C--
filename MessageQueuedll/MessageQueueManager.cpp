#include "MessageQueueManager.h"

MessageQueueManager::MessageQueueManager()
	: m_messageQueue(nullptr)
	, m_maxMessageSize(0)
	, m_maxMessages(0)
	, m_initialized(false)
{
}

MessageQueueManager::~MessageQueueManager()
{
	if (m_messageQueue) {
		message_queue::remove("message_queue");
		delete m_messageQueue;
		m_messageQueue = nullptr;
	}
}

bool MessageQueueManager::initialize(const char* queueName, size_t maxMessages, size_t maxMessageSize)
{
	if (m_initialized) {
		return false;
	}

	try {
		m_messageQueue = new message_queue(open_or_create, queueName, maxMessages, maxMessageSize);
		m_maxMessageSize = maxMessageSize;
		m_maxMessages = maxMessages;
		m_initialized = true;
		return true;
	}
	catch (const interprocess_exception&) {
		m_messageQueue = nullptr;
		return false;
	}
}

bool MessageQueueManager::sendMessage(const std::string& message)
{
	if (!m_initialized || !m_messageQueue) {
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
	if (!m_initialized || !m_messageQueue) {
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
	return m_initialized ? m_messageQueue->get_num_msg() : 0;
}

size_t MessageQueueManager::getMaxMessageSize() const
{
	return m_maxMessageSize;
}

bool MessageQueueManager::isInitialized() const
{
	return m_initialized;
}

size_t MessageQueueManager::getMaxMessages() const
{
	return m_maxMessages;
}
