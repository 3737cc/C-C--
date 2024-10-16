// messagequeues.h
#pragma once

#include "ui_messagequeues.h"
#include <QMainWindow>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <Windows.h>
#include <QMessageBox>
#include <random>

using namespace boost::interprocess;
extern int m_iCurrentQueueMessages; //标志量

class MessageQueues : public QMainWindow
{
	Q_OBJECT
public:
	MessageQueues(QWidget* parent = nullptr);
	~MessageQueues();

private slots:
	void onWriteButtonClicked();
	void onReadButtonClicked();

	std::string generateRandomSring(size_t length);
private:
	Ui::MessageQueuesClass ui;
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