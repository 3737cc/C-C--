// messagequeues.h
#pragma once

#include "ui_messagequeuesB.h"
#include <QMainWindow>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <Windows.h>
#include <QMessageBox>
#include <random>
#include "MessageQueues.h"
using namespace boost::interprocess;

class MessageQueuesB : public QMainWindow
{
	Q_OBJECT
public:
	MessageQueuesB(QWidget* parent = nullptr);
	~MessageQueuesB();

	std::string generateRandomSring(size_t length);

private slots:
	void onWriteButtonClicked();
	void onReadButtonClicked();

private:
	Ui::MessageQueuesBClass ui;
	message_queue* m_mq;
	const int m_iByte;//随机发送消息大小
	const int m_iMaxByte;//最大消息大小
	const int m_iMaxQueueMessages;//消息队列最大消息数量
	LARGE_INTEGER m_frequency; // 计时器频率
	LARGE_INTEGER m_start, m_end; // 计时器值
	//列表值更新
	QTableWidgetItem* m_queue;//队列大小
	QTableWidgetItem* m_byte;
	QTableWidgetItem* m_time;
};