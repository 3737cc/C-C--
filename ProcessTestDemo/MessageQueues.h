// messagequeues.h
#pragma once

#include "ui_messagequeues.h"
#include <QMainWindow>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <Windows.h>
#include <QMessageBox>
#include <random>
#include <QTimer>

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

	std::string generateRandomSring(size_t length);

	void startAutoSending();
	void autoSendMessage();
	void onStartAutoSendingClicked();
	void startAutoReading();
	void autoReadMessage();
	void onStartAutoReadingClicked();
	void stopAuto();

private:
	Ui::MessageQueuesClass ui;
	message_queue* m_mq;
	LARGE_INTEGER m_frequency; // 计时器频率
	LARGE_INTEGER m_start, m_end; // 计时器值
	QTimer* m_timerSend;  // 定时器成员变量
	QTimer* m_timerRead;
	//列表值更新
	const int m_iSetByte;//随机发送消息大小
	int m_iNumQM;
	int m_iMaxNum;
	int m_iMinNum;
	int m_iSumNum;
	int m_iNum;//当前消息数量
	const int m_iMaxByte;//最大消息大小
	const int m_iMaxQueueMessages;//消息队列最大消息数量
	QTableWidgetItem* m_queue;//队列大小
	QTableWidgetItem* m_byte;
	QTableWidgetItem* m_sumQueueMessages;
	QTableWidgetItem* m_numQueueMessages;//剩余数据量
	QTableWidgetItem* m_time;
};