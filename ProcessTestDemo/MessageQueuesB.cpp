// messagequeues.cpp
#include "messagequeuesB.h"
#include <QMessageBox>

MessageQueuesB::MessageQueuesB(QWidget* parent)
	: QMainWindow(parent)
	, m_iByte(255)//定义随机发送消息大小
	, m_iNumQM(10000)
	, m_iMaxByte(255)//定义最大消息大小
	, m_iMaxQueueMessages(100000)//定义消息数量
{
	ui.setupUi(this);

	// 尝试创建或打开消息队列
	if (!m_mq) {
		try {
			// 创建或打开新的消息队列
			m_mq = new message_queue(open_or_create, "message_queue", m_iMaxQueueMessages, m_iMaxByte);
		}
		catch (const interprocess_exception& ex) {
			QMessageBox::critical(this, "Error",
				QString("Failed to create message queue: %1").arg(ex.what()));
			m_mq = nullptr;
		}
	}

	connect(ui.writeButton, &QPushButton::clicked, this, &MessageQueuesB::onWriteButtonClicked);
	connect(ui.readButton, &QPushButton::clicked, this, &MessageQueuesB::onReadButtonClicked);
	connect(ui.autoWriteButton, &QPushButton::clicked, this, &MessageQueuesB::onStartAutoSendingClicked);
	connect(ui.autoReadButton, &QPushButton::clicked, this, &MessageQueuesB::onStartAutoReadingClicked);

	ui.memoryBlockTable->setRowCount(0);
	ui.memoryBlockTable->setColumnCount(3);
}

MessageQueuesB::~MessageQueuesB() {
	if (m_mq) {
		message_queue::remove("message_queue");
		delete m_mq;
	}
}

void MessageQueuesB::onWriteButtonClicked()
{
	if (!m_mq) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	// 检查消息队列中的消息数量是否已达到最大值
	if (int i_iNum = m_mq->get_num_msg() >= m_iMaxQueueMessages) {
		QMessageBox::warning(this, "Warning", "Message queue is full!");
		return;
	}

	QString inputText = ui.valueInput->text();
	std::string inputMessage = inputText.toStdString();
	if (inputMessage.size() > m_iByte) {
		QMessageBox::warning(this, "Warning", "Message is too large!");
		return;
	}

	size_t l_szLength = m_iByte;
	double l_dTotalTime = 0;

	for (int i = 0; i < m_iNumQM; ++i) {
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
		l_dTotalTime += l_dInterval;
	}

	// 获取当前行数，并在末尾插入一行
	int newRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(newRow);

	// 将数据填入新增的行
	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
	m_byte = new QTableWidgetItem(QString::number(m_iByte));
	m_time = new QTableWidgetItem(QString::number(l_dTotalTime));
	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
	ui.memoryBlockTable->setItem(newRow, 2, m_time);

	std::cout << "Total time elapsed for sending messages: " << l_dTotalTime << " milliseconds" << std::endl;
}


void MessageQueuesB::onReadButtonClicked()
{
	if (!m_mq) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	// 检查消息队列中的消息数量是否已达到最大值
	if (int i_iNum = m_mq->get_num_msg() <= 0) {
		QMessageBox::warning(this, "Warning", "Message queue is full!");
		return;
	}

	std::vector<char> l_cBuffer(m_iMaxByte); // 动态分配内存
	size_t l_szReceivedSize = 0;
	unsigned int l_uPriority;
	double l_dTotalTime = 0;
	for (int i = 0; i < m_iNumQM; ++i) {
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
	// 获取当前行数，并在末尾插入一行
	int newRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(newRow);

	// 将数据填入新增的行
	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
	m_byte = new QTableWidgetItem(QString::number(m_iByte));
	m_time = new QTableWidgetItem(QString::number(l_dTotalTime));
	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
	ui.memoryBlockTable->setItem(newRow, 2, m_time);
	std::cout << "Total time elapsed for receiving  messages: " << l_dTotalTime << " milliseconds" << std::endl;
}

//生成随机字符串
std::string MessageQueuesB::generateRandomSring(size_t length) {
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

//自动发送数据
void MessageQueuesB::startAutoSending()
{
	// 设置一个定时器，每隔固定时间发送数据
	auto* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &MessageQueuesB::autoSendMessage);
	timer->start(1000); // 每1秒发送一次数据
}

void MessageQueuesB::autoSendMessage()
{
	if (!m_mq) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	// 检查消息队列中的消息数量是否已达到最大值
	if (m_mq->get_num_msg() >= m_iMaxQueueMessages) {
		QMessageBox::warning(this, "Warning", "Message queue is full!");
		return;
	}

	size_t l_szLength = m_iByte;
	double l_dTotalTime = 0;

	std::string l_szRandomMessage = generateRandomSring(l_szLength);
	for (int i = 0; i < m_iNumQM; ++i) {
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
		l_dTotalTime += l_dInterval;
	}

	// 获取当前行数，并在末尾插入一行
	int newRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(newRow);

	// 填入数据
	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
	m_byte = new QTableWidgetItem(QString::number(m_iByte));
	m_time = new QTableWidgetItem(QString::number(l_dTotalTime));
	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
	ui.memoryBlockTable->setItem(newRow, 2, m_time);

	std::cout << "Message sent. Time elapsed: " << l_dTotalTime << " ms" << std::endl;
}

void MessageQueuesB::onStartAutoSendingClicked()
{
	startAutoSending();
}

void MessageQueuesB::startAutoReading()
{
	// 创建一个定时器，每隔固定时间读取消息
	auto* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &MessageQueuesB::autoReadMessage);
	timer->start(1000); // 每1秒读取一次消息
}

void MessageQueuesB::autoReadMessage()
{
	if (!m_mq) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	// 检查消息队列中是否有消息可读
	if (m_mq->get_num_msg() <= 0) {
		std::cout << "No message available." << std::endl;
		return; // 如果没有消息可读，直接返回
	}

	std::vector<char> l_cBuffer(m_iMaxByte); // 动态分配内存
	size_t l_szReceivedSize = 0;
	unsigned int l_uPriority;
	double l_dTotalTime = 0;

	for (int i = 0; i < m_iNumQM; ++i) {
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
	// 获取当前行数，并在末尾插入一行
	int newRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(newRow);

	// 将数据填入新增的行
	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
	m_byte = new QTableWidgetItem(QString::number(m_iByte));
	m_time = new QTableWidgetItem(QString::number(l_dTotalTime));
	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
	ui.memoryBlockTable->setItem(newRow, 2, m_time);

	std::cout << "Total time elapsed for receiving messages: " << l_dTotalTime << " milliseconds" << std::endl;
}

void MessageQueuesB::onStartAutoReadingClicked()
{
	startAutoReading();  // 点击按钮时启动自动读取数据
}