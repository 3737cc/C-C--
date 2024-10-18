// messagequeues.h
#pragma once

#include "ui_messagequeues.h"
#include <QMainWindow>
#include <iostream>
#include <Windows.h>
#include <QMessageBox>
#include <random>
#include <QTimer>
#include <MessageQueueManager.h>

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
	MessageQueueManager m_queueManager;
	LARGE_INTEGER m_frequency; // 计时器频率
	LARGE_INTEGER m_start, m_end; // 计时器值
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