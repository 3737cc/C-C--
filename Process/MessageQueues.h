// messagequeues.h
#pragma once

#include "ui_messagequeues.h"
#include <QMainWindow>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <Windows.h>

using namespace boost::interprocess;

class MessageQueues : public QMainWindow
{
	Q_OBJECT
public:
	MessageQueues(QWidget* parent = nullptr);
	~MessageQueues();

private slots:
	void onWriteButtonClicked();
	void onReadButtonClicked();

private:
	Ui::MessageQueuesClass ui;
	message_queue* m_mq;
	LARGE_INTEGER frequency; // 计时器频率
	LARGE_INTEGER start, end; // 计时器值
};