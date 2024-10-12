// messagequeues.cpp
#include "messagequeues.h"
#include <QMessageBox>
#include <random>

MessageQueues::MessageQueues(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// 尝试创建消息队列，仅在第一次创建时执行
	if (!m_mq) {
		try {
			// 创建新的消息队列
			m_mq = new message_queue(create_only, "message_queue", 100000, sizeof(char) * 256);
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
	if (inputText.isEmpty()) {
		QMessageBox::warning(this, "Warning", "Input cannot be empty!");
		return;
	}

	std::string inputMessage = inputText.toStdString();
	if (inputMessage.size() > 256) {
		QMessageBox::warning(this, "Warning", "Message is too large!");
		return;
	}

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
	try {
		m_mq->send(inputMessage.c_str(), inputMessage.size(), 0);
		ui.valueInput->clear();
	}
	catch (const interprocess_exception& ex) {
		QMessageBox::critical(this, "Error",
			QString("Failed to send message: %1").arg(ex.what()));
	}
	QueryPerformanceCounter(&end);
	double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
	std::cout << "Time elapsed: " << interval << " milliseconds" << std::endl;

	//// 用于生成随机字符串的字符集
	//const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	//std::default_random_engine generator;
	//std::uniform_int_distribution<size_t> length_dist(1, 255);  // 随机长度分布
	//std::uniform_int_distribution<int> char_dist(0, charset.size() - 1);  // 随机字符分布

	//double total_time = 0;
	//QueryPerformanceFrequency(&frequency);

	//for (int i = 0; i < 2000; ++i) {
	//	// 生成随机消息
	//	std::string randomMessage;
	//	size_t messageLength = length_dist(generator);
	//	for (size_t j = 0; j < messageLength; ++j) {
	//		randomMessage += charset[char_dist(generator)];
	//	}

	//	QueryPerformanceCounter(&start);
	//	try {
	//		m_mq->send(randomMessage.c_str(), randomMessage.size(), 0);
	//	}
	//	catch (const interprocess_exception& ex) {
	//		QMessageBox::critical(this, "Error",
	//			QString("Failed to send message: %1").arg(ex.what()));
	//		return;
	//	}
	//	QueryPerformanceCounter(&end);

	//	// 计算每次消息发送的时间
	//	double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
	//	total_time += interval;
	//}

	//std::cout << "Total time elapsed for sending messages: " << total_time << " milliseconds" << std::endl;
}

void MessageQueues::onReadButtonClicked()
{
	if (!m_mq) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	char buffer[256] = {};;
	size_t received_size = 0;
	unsigned int priority;
	double total_time = 0;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);
	try {
		m_mq->try_receive(buffer, sizeof(buffer), received_size, priority);
		QString outputText = QString::fromLocal8Bit(buffer, received_size);
		ui.valueOutput->setText(outputText);
	}
	catch (const interprocess_exception& ex) {
		QMessageBox::critical(this, "Error",
			QString("Failed to receive message: %1").arg(ex.what()));
	}
	QueryPerformanceCounter(&end);
	double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
	std::cout << "Total time elapsed for receive messages: " << interval << " milliseconds" << std::endl;
	//// 读取 1000 次消息
	   //for (int i = 0; i < 2000; ++i) {
	   //	QueryPerformanceFrequency(&frequency);
	   //	QueryPerformanceCounter(&start);

	   //	try {
	   //		// 尝试接收消息
	   //		if (m_mq->try_receive(buffer, sizeof(buffer), received_size, priority)) {
	   //			// 如果成功接收到消息，显示输出
	   //			QString outputText = QString::fromLocal8Bit(buffer, received_size);
	   //			ui.valueOutput->setText(outputText);
	   //		}
	   //		else {
	   //			// 如果没有消息可接收，可以继续或者退出
	   //			std::cout << "No message available." << std::endl;
	   //			continue;  // 可以根据需求决定是否继续或退出
	   //		}
	   //	}
	   //	catch (const interprocess_exception& ex) {
	   //		QMessageBox::critical(this, "Error",
	   //			QString("Failed to receive message: %1").arg(ex.what()));
	   //		return;
	   //	}

	   //	QueryPerformanceCounter(&end);
	   //	// 计算每次消息接收所用时间
	   //	double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
	   //	total_time += interval;
	   //}

	   //std::cout << "Total time elapsed for receiving 1000 messages: " << total_time << " milliseconds" << std::endl;
}