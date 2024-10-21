// messagequeues.cpp
#include "messagequeues.h"

MessageQueues::MessageQueues(QWidget* parent)
	: QMainWindow(parent)
	, m_iSetByte(255)//定义随机发送消息大小
	, m_iNumQM(1000)
	, m_iMaxByte(255)//定义最大消息大小
	, m_iMaxQueueMessages(100000)//定义消息数量
	, m_numQueueMessages(0)//初始数据数量
	, m_sumQueueMessages(0)
{
	ui.setupUi(this);

	//message_queue::remove("message_queue");
	// 初始化消息队列管理器
	if (!m_queueManager.initialize("messageQueue", m_iMaxQueueMessages, m_iMaxByte)) {
		QMessageBox::critical(this, "Error", "Failed to initialize message queue!");
	}

	connect(ui.writeButton, &QPushButton::clicked, this, &MessageQueues::onWriteButtonClicked);
	connect(ui.readButton, &QPushButton::clicked, this, &MessageQueues::onReadButtonClicked);
	connect(ui.autoWriteButton, &QPushButton::clicked, this, &MessageQueues::onStartAutoSendingClicked);
	connect(ui.autoReadButton, &QPushButton::clicked, this, &MessageQueues::onStartAutoReadingClicked);
	connect(ui.stopButton, &QPushButton::clicked, this, &MessageQueues::stopAuto);

	ui.memoryBlockTable->setRowCount(0);
	ui.memoryBlockTable->setColumnCount(5);
}

MessageQueues::~MessageQueues() {
	//m_queueManager.remove();
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
	m_iMinNum = m_queueManager.getMessageCount();

	QString inputText = ui.writePlainTextEdit->toPlainText();
	std::string inputMessage = inputText.toStdString();
	if (inputMessage.size() > m_iMaxByte) {
		QMessageBox::warning(this, "Warning", "Message is too large!");
		return;
	}

	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);
	try {
		m_queueManager.sendMessage(inputMessage);
	}
	catch (const interprocess_exception& ex) {
		QMessageBox::critical(this, "Error",
			QString("Failed to send message: %1").arg(ex.what()));
		return;
	}
	QueryPerformanceCounter(&m_end);

	// 计算每次消息发送的时间
	double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
	std::cout << "Total time elapsed for sending messages: " << l_dInterval << " milliseconds" << std::endl;

	// 获取当前行数，并在末尾插入一行
	int newRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(newRow);
	m_iNum = m_queueManager.getMessageCount();
	m_iMaxNum = m_queueManager.getMessageCount();

	// 将数据填入新增的行
	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
	m_byte = new QTableWidgetItem(QString::number(inputMessage.size()));
	m_sumQueueMessages = new QTableWidgetItem(QString::number(m_iMaxNum - m_iMinNum));
	m_numQueueMessages = new QTableWidgetItem(QString::number(m_iNum));
	m_time = new QTableWidgetItem(QString::number(l_dInterval));

	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
	ui.memoryBlockTable->setItem(newRow, 2, m_sumQueueMessages);
	ui.memoryBlockTable->setItem(newRow, 3, m_numQueueMessages);
	ui.memoryBlockTable->setItem(newRow, 4, m_time);
	ui.memoryBlockTable->scrollToBottom();
}

void MessageQueues::onReadButtonClicked()
{
	if (!m_queueManager.isInitialized()) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	// 检查消息队列中的消息数量是否已达到最大值
	if (m_queueManager.getMessageCount() <= 0) {
		QMessageBox::warning(this, "Warning", "Message queue is full!");
		return;
	}
	m_iMaxNum = m_queueManager.getMessageCount();
	std::vector<char> l_cBuffer(m_iMaxByte); // 动态分配内存
	size_t l_szReceivedSize = 0;
	unsigned int l_uPriority;
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);

	try {
		// 尝试接收消息
		if (m_queueManager.receiveMessage(l_cBuffer, l_szReceivedSize)) {
			// 如果成功接收到消息，显示输出
			QString outputText = QString::fromLocal8Bit(l_cBuffer.data(), l_szReceivedSize);
			ui.readPlainTextEdit->setPlainText(outputText);
		}
		else {
			// 如果没有消息可接收，可以继续或者退出
			std::cout << "No message available." << std::endl;
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
	std::cout << "Total time elapsed for receiving  messages: " << l_dInterval << " milliseconds" << std::endl;

	// 获取当前行数，并在末尾插入一行
	int newRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(newRow);
	m_iNum = m_queueManager.getMessageCount();
	m_iMinNum = m_queueManager.getMessageCount();

	// 将数据填入新增的行
	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
	m_byte = new QTableWidgetItem(QString::number(l_szReceivedSize));
	m_sumQueueMessages = new QTableWidgetItem(QString::number(m_iMaxNum - m_iMinNum));
	m_numQueueMessages = new QTableWidgetItem(QString::number(m_iNum));
	m_time = new QTableWidgetItem(QString::number(l_dInterval));

	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
	ui.memoryBlockTable->setItem(newRow, 2, m_sumQueueMessages);
	ui.memoryBlockTable->setItem(newRow, 3, m_numQueueMessages);
	ui.memoryBlockTable->setItem(newRow, 4, m_time);
	ui.memoryBlockTable->scrollToBottom();
}

//自动发送数据
void MessageQueues::startAutoSending()
{
	// 设置一个定时器，每隔固定时间发送数据
	m_timerSend = new QTimer(this);
	connect(m_timerSend, &QTimer::timeout, this, &MessageQueues::autoSendMessage);
	m_timerSend->start(1000); // 每1秒发送一次数据
}

void MessageQueues::autoSendMessage()
{
	if (!m_queueManager.isInitialized()) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	if (m_queueManager.getMessageCount() >= m_iMaxQueueMessages) {
		QMessageBox::warning(this, "Warning", "Message queue is full!");
		return;
	}

	size_t l_szLength = m_iSetByte;
	double l_dTotalTime = 0;
	int l_iSum = 0;

	std::string l_szRandomMessage = generateRandomSring(l_szLength);
	for (int i = 0; i < m_iNumQM; ++i) {
		std::string l_szRandomMessage = generateRandomSring(l_szLength);
		m_iMinNum = m_queueManager.getMessageCount();
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_start);
		try {
			m_queueManager.sendMessage(l_szRandomMessage);
		}
		catch (const interprocess_exception& ex) {
			QMessageBox::critical(this, "Error",
				QString("Failed to send message: %1").arg(ex.what()));
			return;
		}
		QueryPerformanceCounter(&m_end);

		m_iMaxNum = m_queueManager.getMessageCount();
		l_iSum += m_iMaxNum - m_iMinNum;
		// 计算每次消息发送的时间
		double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
		l_dTotalTime += l_dInterval;
	}
	std::cout << "Message sent. Time elapsed: " << l_dTotalTime << " ms" << std::endl;
	// 获取当前行数，并在末尾插入一行
	int newRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(newRow);
	m_iNum = m_queueManager.getMessageCount();

	// 将数据填入新增的行
	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
	m_byte = new QTableWidgetItem(QString::number(m_iSetByte));
	m_sumQueueMessages = new QTableWidgetItem(QString::number(l_iSum));
	m_numQueueMessages = new QTableWidgetItem(QString::number(m_iNum));
	m_time = new QTableWidgetItem(QString::number(l_dTotalTime));

	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
	ui.memoryBlockTable->setItem(newRow, 2, m_sumQueueMessages);
	ui.memoryBlockTable->setItem(newRow, 3, m_numQueueMessages);
	ui.memoryBlockTable->setItem(newRow, 4, m_time);
	ui.memoryBlockTable->scrollToBottom();
}

void MessageQueues::onStartAutoSendingClicked()
{
	startAutoSending();
	//startAutoReading();  // 点击按钮时启动自动读取数据
}

void MessageQueues::startAutoReading()
{
	// 创建一个定时器，每隔固定时间读取消息
	m_timerRead = new QTimer(this);
	connect(m_timerRead, &QTimer::timeout, this, &MessageQueues::autoReadMessage);
	m_timerRead->start(1000); // 每1秒读取一次消息
}

void MessageQueues::autoReadMessage()
{
	if (!m_queueManager.isInitialized()) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	// 检查消息队列中的消息数量是否已达到最大值
	if (m_queueManager.getMessageCount() <= 0) {
		QMessageBox::warning(this, "Warning", "Message queue is full!");
		return;
	}

	std::vector<char> l_cBuffer(m_iMaxByte); // 动态分配内存
	size_t l_szReceivedSize = 0;
	unsigned int l_uPriority;
	double l_dTotalTime = 0;
	int l_iSum = 0;

	for (int i = 0; i < m_iNumQM; ++i) {
		m_iMaxNum = m_queueManager.getMessageCount();
		QueryPerformanceFrequency(&m_frequency);
		QueryPerformanceCounter(&m_start);

		try {
			// 尝试接收消息
			if (m_queueManager.receiveMessage(l_cBuffer, l_szReceivedSize)) {
				// 如果成功接收到消息，显示输出
				QString outputText = QString::fromLocal8Bit(l_cBuffer.data(), l_szReceivedSize);
				/*ui.readPlainTextEdit->setPlainText(outputText);*/
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
		m_iMinNum = m_queueManager.getMessageCount();
		l_iSum += m_iMaxNum - m_iMinNum;
		// 计算每次消息接收所用时间
		double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
		l_dTotalTime += l_dInterval;
	}
	std::cout << "Total time elapsed for receiving messages: " << l_dTotalTime << " milliseconds" << std::endl;
	// 获取当前行数，并在末尾插入一行
	int newRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(newRow);
	m_iNum = m_queueManager.getMessageCount();

	// 将数据填入新增的行
	m_queue = new QTableWidgetItem(QString::number(m_iMaxQueueMessages * m_iMaxByte));
	m_byte = new QTableWidgetItem(QString::number(m_iSetByte));
	m_sumQueueMessages = new QTableWidgetItem(QString::number(l_iSum));
	m_numQueueMessages = new QTableWidgetItem(QString::number(m_iNum));
	m_time = new QTableWidgetItem(QString::number(l_dTotalTime));

	ui.memoryBlockTable->setItem(newRow, 0, m_queue);
	ui.memoryBlockTable->setItem(newRow, 1, m_byte);
	ui.memoryBlockTable->setItem(newRow, 2, m_sumQueueMessages);
	ui.memoryBlockTable->setItem(newRow, 3, m_numQueueMessages);
	ui.memoryBlockTable->setItem(newRow, 4, m_time);
	ui.memoryBlockTable->scrollToBottom();
}

void MessageQueues::onStartAutoReadingClicked()
{
	startAutoReading();  // 点击按钮时启动自动读取数据
}

void MessageQueues::stopAuto()
{
	m_timerSend->stop();
	m_timerRead->stop();  // 停止定时器
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
