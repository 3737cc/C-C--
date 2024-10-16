// messagequeues.cpp
#include "messagequeuesB.h"
#include <QMessageBox>

MessageQueuesB::MessageQueuesB(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// 尝试创建或打开消息队列
	if (!m_mq) {
		try {
			// 创建或打开新的消息队列
			m_mq = new message_queue(open_or_create, "message_queue", 2, 4 * 1024 * 1024);
		}
		catch (const interprocess_exception& ex) {
			QMessageBox::critical(this, "Error",
				QString("Failed to create message queue: %1").arg(ex.what()));
			m_mq = nullptr;
		}
	}

	connect(ui.writeButton, &QPushButton::clicked, this, &MessageQueuesB::onWriteButtonClicked);
	connect(ui.readButton, &QPushButton::clicked, this, &MessageQueuesB::onReadButtonClicked);
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

	try {
		m_mq->send(inputMessage.c_str(), inputMessage.size(), 0);
		ui.valueInput->clear();
	}
	catch (const interprocess_exception& ex) {
		QMessageBox::critical(this, "Error",
			QString("Failed to send message: %1").arg(ex.what()));
	}
}

void MessageQueuesB::onReadButtonClicked()
{
	if (!m_mq) {
		QMessageBox::critical(this, "Error", "Message queue is not initialized!");
		return;
	}

	std::vector<char> l_cBuffer(4 * 1024 * 1024);
	//char buffer[256] = {};
	size_t l_szReceivedSize = 0;
	unsigned int l_uPriority;

	try {
		m_mq->try_receive(l_cBuffer.data(), l_cBuffer.size(), l_szReceivedSize, l_uPriority);
		QString outputText = QString::fromLocal8Bit(l_cBuffer.data(), l_szReceivedSize);
		ui.valueOutput->setText(outputText);
	}
	catch (const interprocess_exception& ex) {
		QMessageBox::critical(this, "Error",
			QString("Failed to receive message: %1").arg(ex.what()));
	}
}