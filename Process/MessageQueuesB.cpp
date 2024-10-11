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
			// 创建新的消息队列
			m_mq = new message_queue(open_or_create, "message_queue", 100, sizeof(char) * 256);
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

	char buffer[256] = {};
	size_t received_size = 0;
	unsigned int priority;

	try {
		m_mq->try_receive(buffer, sizeof(buffer), received_size, priority);
		QString outputText = QString::fromLocal8Bit(buffer, received_size);
		ui.valueOutput->setText(outputText);
	}
	catch (const interprocess_exception& ex) {
		QMessageBox::critical(this, "Error",
			QString("Failed to receive message: %1").arg(ex.what()));
	}
}