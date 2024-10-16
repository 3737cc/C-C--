// messagequeues.cpp
#include "messagequeues.h"

MessageQueues::MessageQueues(QWidget* parent)
	: QMainWindow(parent),
	m_iByte(4 * 1024 * 1024)
{
	ui.setupUi(this);

	message_queue::remove("message_queue");
	// 尝试创建消息队列，仅在第一次创建时执行
	if (!m_mq) {
		try {
			// 创建新的消息队列
			m_mq = new message_queue(create_only, "message_queue", 2, m_iByte);
		}
		catch (const interprocess_exception& ex) {
			QMessageBox::critical(this, "Error",
				QString("Failed to create message queue: %1").arg(ex.what()));
			m_mq = nullptr;
		}
	}

	connect(ui.writeButton, &QPushButton::clicked, this, &MessageQueues::onWriteButtonClicked);
	connect(ui.readButton, &QPushButton::clicked, this, &MessageQueues::onReadButtonClicked);
}

MessageQueues::~MessageQueues() {
	if (m_mq) {
		message_queue::remove("message_queue");
		delete m_mq;
	}
}

void MessageQueues::onWriteButtonClicked()
{
	if (!m_mq) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	QString inputText = ui.valueInput->text();

	std::string inputMessage = inputText.toStdString();
	if (inputMessage.size() > m_iByte) {
		QMessageBox::warning(this, "Warning", "Message is too large!");
		return;
	}
	size_t l_szLength = m_iByte;
	double l_dTotal_time = 0;

	for (int i = 0; i < 1; ++i) {
		std::string l_szRandomMessage = generateRandomSring(l_szLength);
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_start);
		try {
			m_mq->send(l_szRandomMessage.c_str(), l_szRandomMessage.size(), 0);
		}
		catch (const interprocess_exception& ex) {
			QMessageBox::critical(this, "Error",
				QString("Failed to send message: %1").arg(ex.what()));
			return;
		}
		QueryPerformanceCounter(&m_end);

		// 计算每次消息发送的时间
		double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
		l_dTotal_time += l_dInterval;
	}

	std::cout << "Total time elapsed for sending messages: " << l_dTotal_time << " milliseconds" << std::endl;
}

void MessageQueues::onReadButtonClicked()
{
	if (!m_mq) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	std::vector<char> l_cBuffer(m_iByte); // 动态分配内存
	size_t l_szReceivedSize = 0;
	unsigned int l_uPriority;
	double l_dTotalTime = 0;
	for (int i = 0; i < 1; ++i) {
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_start);

		try {
			// 尝试接收消息
			if (m_mq->try_receive(l_cBuffer.data(), l_cBuffer.size(), l_szReceivedSize, l_uPriority)) {
				// 如果成功接收到消息，显示输出
				QString outputText = QString::fromLocal8Bit(l_cBuffer.data(), l_szReceivedSize);
				//ui.valueOutput->setText(outputText);
			}
			else {
				// 如果没有消息可接收，可以继续或者退出
				std::cout << "No message available." << std::endl;
				continue;  // 可以根据需求决定是否继续或退出
			}
		}
		catch (const interprocess_exception& ex) {
			QMessageBox::critical(this, "Error",
				QString("Failed to receive message: %1").arg(ex.what()));
			return;
		}

		QueryPerformanceCounter(&m_end);
		// 计算每次消息接收所用时间
		double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
		l_dTotalTime += l_dInterval;
	}

	std::cout << "Total time elapsed for receiving  messages: " << l_dTotalTime << " milliseconds" << std::endl;
}

//生成随机字符串
std::string MessageQueues::generateRandomSring(size_t length) {
	std::string l_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string l_Result;
	l_Result.reserve(length);

	std::random_device l_rd;
	std::mt19937 generator(l_rd());
	std::uniform_int_distribution<> dist(0, l_chars.size() - 1);

	for (size_t i = 0; i < length; ++i) {
		l_Result += l_chars[dist(generator)];
	}

	return l_Result;
}