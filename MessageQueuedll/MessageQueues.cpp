// messagequeues.cpp
#include "messagequeues.h"

MessageQueues::MessageQueues(QWidget* parent)
	: QMainWindow(parent)
	, m_iByte(255)//定义随机发送消息大小
	, m_iNumQM(10000)
	, m_iMaxByte(255)//定义最大消息大小
	, m_iMaxQueueMessages(100000)//定义消息数量
	//, m_iCurrentQueueMessages(0)//跟踪消息数量
{
	ui.setupUi(this);

	// 初始化消息队列管理器
	if (!m_queueManager.initialize("message_queue", m_iMaxQueueMessages, m_iMaxByte)) {
		QMessageBox::critical(this, "Error", "Failed to initialize message queue!");
	}

	connect(ui.writeButton, &QPushButton::clicked, this, &MessageQueues::onWriteButtonClicked);
	connect(ui.readButton, &QPushButton::clicked, this, &MessageQueues::onReadButtonClicked);
	//connect(ui.autoWriteButton, &QPushButton::clicked, this, &MessageQueues::onStartAutoSendingClicked);
	//connect(ui.autoReadButton, &QPushButton::clicked, this, &MessageQueues::onStartAutoReadingClicked);

	ui.memoryBlockTable->setRowCount(0);
	ui.memoryBlockTable->setColumnCount(3);
}

MessageQueues::~MessageQueues() {

}

void MessageQueues::onWriteButtonClicked()
{
	if (!m_queueManager.isInitialized()) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	if (m_queueManager.getMessageCount() >= m_iMaxQueueMessages) {
		QMessageBox::warning(this, "Warning", "Message queue is full!");
		return;
	}

	QString inputText = ui.valueInput->text();
	std::string inputMessage = inputText.toStdString();
	if (inputMessage.size() > m_iByte) {
		QMessageBox::warning(this, "Warning", "Message is too large!");
		return;
	}

	double l_dTotalTime = 0;

	for (int i = 0; i < m_iNumQM; ++i) {
		std::string l_szRandomMessage = generateRandomSring(m_iByte);

		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_start);

		if (!m_queueManager.sendMessage(l_szRandomMessage)) {
			QMessageBox::critical(this, "Error", "Failed to send message!");
			return;
		}

		QueryPerformanceCounter(&m_end);
		double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
		l_dTotalTime += l_dInterval;
	}

	// 更新表格
	int newRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(newRow);

	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
	m_byte = new QTableWidgetItem(QString::number(m_iByte));
	m_time = new QTableWidgetItem(QString::number(l_dTotalTime));
	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
	ui.memoryBlockTable->setItem(newRow, 2, m_time);

	std::cout << "Total time elapsed for sending messages: " << l_dTotalTime << " milliseconds" << std::endl;
}

void MessageQueues::onReadButtonClicked()
{
	// 检查消息队列是否初始化
	if (!m_queueManager.isInitialized()) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	// 检查消息队列是否为空
	if (m_queueManager.getMessageCount() <= 0) {
		QMessageBox::warning(this, "Warning", "Message queue is empty!");
		return;
	}

	// 准备缓冲区和计时变量
	std::vector<char> l_cBuffer;
	size_t l_szReceivedSize = 0;
	double l_dTotalTime = 0;

	// 读取消息循环
	for (int i = 0; i < m_iNumQM; ++i) {
		// 开始计时
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_start);

		// 尝试接收消息
		if (m_queueManager.receiveMessage(l_cBuffer, l_szReceivedSize)) {
			//QString outputText = QString::fromLocal8Bit(l_cBuffer.data(), l_szReceivedSize);
			//ui.valueOutput->setText(outputText);
		}
		else {
			std::cout << "No message available." << std::endl;
			continue;
		}

		// 结束计时并计算时间
		QueryPerformanceCounter(&m_end);
		double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
		l_dTotalTime += l_dInterval;
	}

	// 更新表格显示
	int newRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(newRow);

	// 创建并设置表格项
	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
	m_byte = new QTableWidgetItem(QString::number(m_iByte));
	m_time = new QTableWidgetItem(QString::number(l_dTotalTime));

	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
	ui.memoryBlockTable->setItem(newRow, 2, m_time);

	// 输出总耗时
	std::cout << "Total time elapsed for receiving messages: " << l_dTotalTime << " milliseconds" << std::endl;
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

//自动发送数据
//void MessageQueues::startAutoSending()
//{
//	// 设置一个定时器，每隔固定时间发送数据
//	auto* timer = new QTimer(this);
//	connect(timer, &QTimer::timeout, this, &MessageQueues::autoSendMessage);
//	timer->start(1000); // 每1秒发送一次数据
//}

//void MessageQueues::autoSendMessage()
//{
//	if (!m_mq) {
//		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
//		return;
//	}
//
//	// 检查消息队列中的消息数量是否已达到最大值
//	if (m_mq->getNumMsg() >= m_iMaxQueueMessages) {
//		QMessageBox::warning(this, "Warning", "Message queue is full!");
//		return;
//	}
//
//	size_t l_szLength = m_iByte;
//	double l_dTotalTime = 0;
//
//	std::string l_szRandomMessage = generateRandomSring(l_szLength);
//	for (int i = 0; i < m_iNumQM; ++i) {
//		std::string l_szRandomMessage = generateRandomSring(l_szLength);
//		QueryPerformanceFrequency(&m_frequency);
//		QueryPerformanceCounter(&m_start);
//		//try {
//		m_mq->send(l_szRandomMessage.c_str(), l_szRandomMessage.size());
//		//}
//		//catch (const interprocess_exception& ex) {
//		//	QMessageBox::critical(this, "Error",
//		//		QString("Failed to send message: %1").arg(ex.what()));
//		//	return;
//		//}
//		QueryPerformanceCounter(&m_end);
//
//		// 计算每次消息发送的时间
//		double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
//		l_dTotalTime += l_dInterval;
//	}
//
//	// 获取当前行数，并在末尾插入一行
//	int newRow = ui.memoryBlockTable->rowCount();
//	ui.memoryBlockTable->insertRow(newRow);
//
//	// 填入数据
//	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
//	m_byte = new QTableWidgetItem(QString::number(m_iByte));
//	m_time = new QTableWidgetItem(QString::number(l_dTotalTime));
//	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
//	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
//	ui.memoryBlockTable->setItem(newRow, 2, m_time);
//
//	std::cout << "Message sent. Time elapsed: " << l_dTotalTime << " ms" << std::endl;
//}

//void MessageQueues::onStartAutoSendingClicked()
//{
//	startAutoSending();
//	//startAutoReading();  // 点击按钮时启动自动读取数据
//}
//
//void MessageQueues::startAutoReading()
//{
//	// 创建一个定时器，每隔固定时间读取消息
//	auto* timer = new QTimer(this);
//	connect(timer, &QTimer::timeout, this, &MessageQueues::autoReadMessage);
//	timer->start(1000); // 每1秒读取一次消息
//}
//
//void MessageQueues::autoReadMessage()
//{
//	if (!m_mq) {
//		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
//		return;
//	}
//
//	// 检查消息队列中是否有消息可读
//	if (m_mq->getNumMsg() <= 0) {
//		std::cout << "No message available." << std::endl;
//		return; // 如果没有消息可读，直接返回
//	}
//
//	std::vector<char> l_cBuffer(m_iMaxByte); // 动态分配内存
//	size_t l_szReceivedSize = 0;
//	unsigned int l_uPriority;
//	double l_dTotalTime = 0;
//
//	for (int i = 0; i < m_iNumQM; ++i) {
//		QueryPerformanceFrequency(&m_frequency);
//		QueryPerformanceCounter(&m_start);
//
//		//try {
//		//	// 尝试接收消息
//		if (m_mq->receive(l_cBuffer.data(), l_cBuffer.size())) {
//			// 如果成功接收到消息，显示输出
//			QString outputText = QString::fromLocal8Bit(l_cBuffer.data(), l_szReceivedSize);
//			//ui.valueOutput->setText(outputText);
//		}
//		else {
//			// 如果没有消息可接收，可以继续或者退出
//			std::cout << "No message available." << std::endl;
//			continue;  // 可以根据需求决定是否继续或退出
//		}
//		//}
//		//catch (const interprocess_exception& ex) {
//		//	QMessageBox::critical(this, "Error",
//		//		QString("Failed to receive message: %1").arg(ex.what()));
//		//	return;
//		//}
//
//		QueryPerformanceCounter(&m_end);
//		// 计算每次消息接收所用时间
//		double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
//		l_dTotalTime += l_dInterval;
//	}
//	// 获取当前行数，并在末尾插入一行
//	int newRow = ui.memoryBlockTable->rowCount();
//	ui.memoryBlockTable->insertRow(newRow);
//
//	// 将数据填入新增的行
//	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
//	m_byte = new QTableWidgetItem(QString::number(m_iByte));
//	m_time = new QTableWidgetItem(QString::number(l_dTotalTime));
//	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
//	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
//	ui.memoryBlockTable->setItem(newRow, 2, m_time);
//
//	std::cout << "Total time elapsed for receiving messages: " << l_dTotalTime << " milliseconds" << std::endl;
//}
//
//void MessageQueues::onStartAutoReadingClicked()
//{
//	startAutoReading();  // 点击按钮时启动自动读取数据
//}