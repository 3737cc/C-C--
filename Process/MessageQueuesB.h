// messagequeues.h
#pragma once

#include "ui_messagequeuesB.h"
#include <QMainWindow>
#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;

class MessageQueuesB : public QMainWindow
{
	Q_OBJECT
public:
	MessageQueuesB(QWidget* parent = nullptr);
	~MessageQueuesB();

private slots:
	void onWriteButtonClicked();
	void onReadButtonClicked();

private:
	Ui::MessageQueuesBClass ui;
	message_queue* m_mq;
};