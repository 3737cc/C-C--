#include "MessageQueueManager.h"

MessageQueueManager::MessageQueueManager(QObject* parent)
	: m_messageQueue(nullptr)
	, m_maxMessageSize(1024)
	, m_maxMessages(10000)
	, m_bInitialized(false)
	, m_autoReceiving(false)
	, m_chMessageQueue("messageQueue")
	, m_hasPrintedEmptyMessage(false)
{
	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &MessageQueueManager::autoReceiveMessages);
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

bool MessageQueueManager::sendData(const std::string& message)
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

// 启动自动获取
void MessageQueueManager::startAutoReceive(int interval) {
	if (!m_autoReceiving) {
		m_autoReceiving = true;
		m_timer->start(interval);
	}
}

// 停止自动获取
void MessageQueueManager::stopAutoReceive() {
	if (m_autoReceiving) {
		m_timer->stop();
		m_autoReceiving = false;
	}
}

void MessageQueueManager::autoReceiveMessages() {
	// 获取当前时间
	QDateTime currentDateTime = QDateTime::currentDateTime();
	if (hasMessages()) { // 只在有消息的情况下进行获取
		std::vector<char> buffer(m_maxMessageSize);
		size_t receivedSize = 0;
		unsigned int priority;
		if (m_messageQueue->try_receive(buffer.data(), buffer.size(), receivedSize, priority)) {
			// 打印当前时间（年月日）
			std::cout << "Received data at: "
				<< currentDateTime.toString("yyyy-MM-dd HH:mm:ss").toStdString() // 年-月-日 时:分:秒
				<< ", Size: " << receivedSize << " bytes." << std::endl;

			// 打印获取的数据内容
			std::cout << "Received data: ";
			for (size_t i = 0; i < receivedSize; ++i) {
				std::cout << buffer[i]; // 逐个字符输出
			}
			std::cout << std::endl; // 换行

			processMessage(buffer, receivedSize);
		}
		m_hasPrintedEmptyMessage = false;
	}
	else {
		if (!m_hasPrintedEmptyMessage) {
			std::cout << "Received data at: "
				<< currentDateTime.toString("yyyy-MM-dd HH:mm:ss").toStdString() // 年-月-日 时:分:秒
				<< ", No messages available." << std::endl;
			m_hasPrintedEmptyMessage = true;
		}
	}
}

bool MessageQueueManager::hasMessages() {
	// 检查队列是否有消息的逻辑
	if (!m_messageQueue) {
		// 处理未初始化的消息队列情况
		return false;
	}

	return m_messageQueue->get_num_msg() > 0; // 返回消息数量是否大于 0
}

void MessageQueueManager::processMessage(const std::vector<char>& buffer, size_t receivedSize) {
	// 将接收到的消息数据转换为 std::string
	std::string message(buffer.data(), receivedSize);
	emit messageReceived(QString::fromStdString(message));//发送信号
}
